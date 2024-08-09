#include "cc1110_regs.h"
#include "aes.h"
#include "dma.h"
#include "stringx.h"


void aes_init() {
    // DMA OUTPUT CHANNEL
    // Configure the DMA output channel for the AES peripheral
    // The transfers are all the same type
    dma_configure_transfer(
        // This is the "AES out" DMA channel
        dma_channel_aes_out,
        // Data is read from ENCDO (AES output register)
        &X_ENCDO,
        // Leave the address pointer blank for now. We set it later
        0,
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
        AES_BLOCK_SIZE);

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
}


/**
 * @brief Load a block of data on the AES DMA channel
 *
 * @pre aes_init
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
 * @brief Compute the cipher block chaining message authentication code (CBC-MAC) of a block of data
 *
 * In cryptography, a cipher block chaining message authentication code is a technique for
 * constructing a message authentication code from a block cipher.
 * The message is encrypted with some block cipher algorithm in CBC mode to create a
 * chain of blocks such that each block depends on the proper encryption of the previous block
 * To calculate the CBC-MAC of message m, one encrypts m in CBC mode with zero initialization
 * vector and keeps the last block
 *
 * @pre aes_init
 * @param data the data to compute the mac of
 * @param data_len length of data in bytes
 * @param key location of key, must be 16 bytes
 * @param result location to put result, must be at least 16 bytes
 */
void aes_compute_cbc_mac(__xdata uint8_t *data, uint16_t data_len, __xdata uint8_t *key, __xdata uint8_t *result) {
    uint16_t i;

    // final block to encrypt, used to add padding to data if it's not a multiple of AES_BLOCK_SIZE
    __xdata uint8_t padded_block[AES_BLOCK_SIZE];
    uint8_t remaining_data_len;
    uint16_t num_bytes_without_pad;

    // padding is 0's
    memsetx(padded_block, 0, sizeof(padded_block));
    remaining_data_len = data_len % AES_BLOCK_SIZE;
    num_bytes_without_pad = data_len - remaining_data_len;

    dma_configure_dest_addr(dma_channel_aes_out, result);

    // Load the key into the AES peripheral
    aes_load_block(key, ENCCS_MODE_CBC_MAC | ENCCS_CMD_LOAD_KEY);

    // Load the nonce, reuse padded_block for all zeros before we memcpy data into it
    aes_load_block(padded_block, ENCCS_MODE_CBC_MAC | ENCCS_CMD_LOAD_IV_NONCE);

    memcpyx(padded_block, data + data_len - remaining_data_len, remaining_data_len);

    // if no padding needed, we get the cbc-mac out when we send the last (full) block
    // if padding, we get it when we send padded_block, after all the full blocks

    // send up to and including second last full block
    for (i = 0; i + AES_BLOCK_SIZE < num_bytes_without_pad; i += AES_BLOCK_SIZE) {
        aes_load_block(data + i, ENCCS_MODE_CBC_MAC | ENCCS_CMD_ENCRYPT_BLOCK);
    }

    // if no padding needed, we get cbc-mac here
    if (remaining_data_len == 0) {
        dma_arm(dma_channel_aes_out);
        aes_load_block(data + i, ENCCS_MODE_CBC | ENCCS_CMD_ENCRYPT_BLOCK);
        dma_wait(dma_channel_aes_out);
        return;
    }

    // else, if there's padding we send last full block and then get cbc-mac from padded block
    if (num_bytes_without_pad > 0) {
        aes_load_block(data + i, ENCCS_MODE_CBC_MAC | ENCCS_CMD_ENCRYPT_BLOCK);
    }

    dma_arm(dma_channel_aes_out);
    aes_load_block(padded_block, ENCCS_MODE_CBC | ENCCS_CMD_ENCRYPT_BLOCK);
    dma_wait(dma_channel_aes_out);
}
