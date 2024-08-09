// OpenLST
// Copyright (C) 2018 Planet Labs Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
/**
 * @file signature.c
 * @brief Routines to perform digitial signature verification on application firmware
 *
 * See application note: Using AES Encryption in CC111xFx and CC251xFx (Rev. C)
 * to use the CBC-MAC feature of the AES co-processor as our signature
 *
 * @author Timothy Wriglesworth
 * @date May 11, 2021
 */

// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use HOME in this case as an
// identifier
#pragma codeseg HOME
#include "signature.h"
#include "compiler_utils.h"
#include "cc1110_regs.h"
#include "dma.h"
#include "flash.h"
#include "stringx.h"

STATIC_ASSERT(flash_size_no_aes_padding, (FLASH_APP_SIGNATURE - FLASH_APP_START) % AES_BLOCK_SIZE == 0);

// The key region contains SIGNATURE_KEY_COUNT keys
// that all default to all ones (FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
__code __at (FLASH_SIGNATURE_KEYS) signature_key_t signature_keys[SIGNATURE_KEY_COUNT];
__code __at (FLASH_APP_SIGNATURE) signature_key_t flash_signature;

/**
 * @brief Load a block of data on the AES DMA channel
 *
 * @pre DMA must be initialized
 * @param[in] addr The memory block to be encrypted
 * @param[in] enccs The Encryption/decryption mode
 */
static void aes_load_block(__xdata void *addr, uint8_t enccs) {

    // Get the src address high and low byte so we can transfer the
    // data we want via the DMA channel
    dma_configure_source_addr(dma_channel_aes_in, addr);

    // Monitor AES coprocessor (ENCCS.RDY) to wait until key is downloaded
    while (!(ENCCS & ENCCS_RDY));

    // Arm the DMA input channel
    dma_arm(dma_channel_aes_in);
    // In order for the AES coprocessor to generate an associated CPU interrupt request
    // (S0CON.ENCIF= 1,) both AES download and AES upload must be completed. For
    // instance, if the DMA upload channel is not armed when starting the AES encryption and
    // decryption, this causes lack of AES ISR execution.
    S0CON &= ~(S0CON_ENCIF);

    // Write to the encryption control register
    ENCCS = enccs | ENCCS_ST;

    // Wait for the DMA transfer to complete
    dma_wait(dma_channel_aes_in);

    // Wait for a AES ISR request
    while (!(S0CON & S0CON_ENCIF));
}

/**
 * @brief Check if application firmware signature is valid
 *
 * In order to use the DMA controller to support the AES coprocessor, two DMA channels must be allocated
 * and configured: one for downloading data from SoC memory to the AES coprocessor and another for
 * uploading processed data from the AES coprocessor to SoC memory
 *
 * To encrypt and decrypt data, the following procedure must be followed:
 * 1. Load key
 * 2. Load initialization vector (IV)/NONCE
 * 3. Download and upload data for encryption and decryption purpose
 *
 * @pre DMA must be initialized
 * @return Signature is valid bit (0 or 1)
 */
__bit signature_app_valid(void) {
    __bit signature_match;

    // Output of AES co-processor stored here
    // This is just a buffer/array containing the calculated signature
    __xdata signature_key_t signature;
    uint8_t i, z;

    //Encryption => enccs = 00b, decryption mode => encss = 01b.
    uint8_t enccs;

    // Pointer for the chunk of application.
    __xdata uint8_t *app_chunk;

    // IV/NONCE buffer
    __xdata uint8_t zeros[AES_BLOCK_SIZE];

    // This is the Initialization vector/ NONCE
    // Set IV/NONCE is all 0's for CBC-MAC Mode
    // CBC-MAC described below
    for (z = 0; z < AES_BLOCK_SIZE; z++) {
        zeros[z] = 0;
    }

    // DMA OUTPUT CHANNEL
    // Configure the DMA output channel for the AES peripheral to output
    // signatures to the signature variable
    // The transfers are all the same type
    dma_configure_transfer(
        // This is the "AES out" DMA channel
        dma_channel_aes_out,
        // Data is read from ENCDO (AES output register)
        &X_ENCDO,
        // and written to signature
        (__xdata void *) signature,
        // 8 bit transfer size per the peripheral spec
        DMA_WORDSIZE_8_BIT |
        // Do a single 16 byte transfer each time we ARM
        DMA_TMODE_SINGLE |
        // Trigger on the AES upload DMA event
        DMA_TRIG_ENC_UP,
        // Don't increment the source address (it's ENCDO)
        DMA_SRCINC_ZERO |
        // Single increment after each byte written to signature
        DMA_DESTINC_ONE |
        // High priority since we don't want the timing
        // to depend on RF/UART events and this is a
        // blocking routine
        DMA_PRIORITY_HIGH);
    // The DMA transfer length is one AES-128 block (16 bytes)
    dma_configure_length(
        dma_channel_aes_out,
        DMA_VLEN_FIXED_USE_LEN,
        sizeof(signature));

    // We're going to try all keys rather than short-circuiting
    // after finding a match to provide insulation against
    // potential timing attacks
    signature_match = 0;

    // The DMA AES input channel reads in one AES-128 block at
    // a time, so we can configure the length here for all
    // transfers
    dma_configure_transfer(
        dma_channel_aes_in,
        // Leave the address pointer blank for now. We set it later
        // It is set via pointer access to the DMA controller configuration structure
        // This will be the data address we send to be checked for correct
        // signature
        0,
        // Data is written to the encryption processor input register
        &X_ENCDI,

        // 8 bit transfer size per the peripheral spec
        DMA_WORDSIZE_8_BIT |

        // Do a single 16 byte transfer each time we ARM
        DMA_TMODE_SINGLE |

        // Trigger on the AES upload DMA event
        DMA_TRIG_ENC_DW,

        // Single increment after each byte written to signature
        DMA_SRCINC_ONE |

        // Don't increment the source address (it's ENCDI)
        DMA_DESTINC_ZERO |

        // High priority since we don't want the timing
        // to depend on RF/UART events and this is a
        // blocking routine
        DMA_PRIORITY_HIGH);


    // The DMA transfer length is one AES-128 block (16 bytes)
    // This is a fixed length
    dma_configure_length(
        dma_channel_aes_in,
        DMA_VLEN_FIXED_USE_LEN,
        AES_BLOCK_SIZE);

    // Control loop to go through cipher keys used in signing
    for (i = 0; i < SIGNATURE_KEY_COUNT; i++) {
        // 1. Load the key into the AES peripheral
        aes_load_block(
            (__xdata void *) signature_keys[i].key,
            ENCCS_MODE_CBC_MAC | ENCCS_CMD_LOAD_KEY);

        // 2. Load the nonce (all zeros)
        aes_load_block(
            (__xdata void *) zeros,
            ENCCS_MODE_CBC_MAC | ENCCS_CMD_LOAD_IV_NONCE);

        // Compute the cipher block chaining message authentication code
        // (CBC-MAC) of the application code

        // Note: In cryptography, a cipher block chaining message authentication
        // code is a technique for constructing a message authentication code from a block cipher.
        // The message is encrypted with some block cipher algorithm in CBC mode to create a
        // chain of blocks such that each block depends on the proper encryption of the previous block
        // To calculate the CBC-MAC of message m, one encrypts m in CBC mode with zero initialization
        // vector and keeps the last block
        // First configure DMA transfers of one AES block
        // at a time into from the flash to the AES peripheral
        // We loop through the application code as specified by
        // the memory map in flash_constants.h from start to end address
        for (app_chunk = (__xdata uint8_t *) FLASH_APP_START;
             app_chunk < (__xdata uint8_t *) FLASH_APP_SIGNATURE;
             app_chunk += AES_BLOCK_SIZE) {

            // The first N-1 blocks (of N) need to be in CBC_MAC mode.
            // This mode does not produce an output. For the last
            // block, we switch to CBC mode which will output the ciphertext
            // of block N, which is the CBC-MAC signature
            if (app_chunk < (__xdata uint8_t *) (FLASH_APP_SIGNATURE - AES_BLOCK_SIZE)) {
                // Not the last page - kick off a transfer in CBC MAC mode
                enccs = ENCCS_MODE_CBC_MAC | ENCCS_CMD_ENCRYPT_BLOCK;
            } else {
                // Last page - set up the upload of the MAC
                // and run in mode CBC to read out the signature
                dma_arm(dma_channel_aes_out);
                // Run in mode CBC to read out the signature
                enccs = ENCCS_MODE_CBC | ENCCS_CMD_ENCRYPT_BLOCK;
            }
            aes_load_block(app_chunk, enccs);
        }
        // Wait for the signature to be read out of the AES peripheral
        dma_wait(dma_channel_aes_out);

        // We compare the newly calculated signature to the flash_signature
        // The flash_signature comes with the new application firmware and was calculated
        // with the same KEYS.
        // As long as no one knows any of the KEYS stored at FLASH_SIGNATURE_KEYS
        // We can be confident the firmware received is the firmware sent by us, if the firmware
        // was altered in anyway, the signatures would not match.
        if (memcmpx_ct((__xdata void *) signature, (__xdata void *) flash_signature, AES_BLOCK_SIZE) == 0) {
            // Set the signature_match flag high
            signature_match = 1;
        }
    }
    return signature_match;
}
