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
 * @file radio.c
 * @brief Radio setup and interrupt service routines
 * @author Timothy Wriglesworth
 * @date May 17, 2021
 */

#ifdef BOOTLOADER
#pragma codeseg APP_UPDATER
#endif

#include "board_defaults.h"
#include "cc1110_regs.h"
#include "crc16.h"
#include "dma.h"
#include "radio.h"
#include "stringx.h"

#ifndef BOOTLOADER
#include "timers.h"
#include "aes.h"
#include "authent.h"
#include "flash_vars.h"
#endif


// Radio register settings for transmit and receive
uint8_t radio_mode_tx;
uint8_t radio_mode_rx;

// If set to 1, radio will be in IDLE mode when not transmitting
__bit radio_disable_rx = 0;


// Volatile keyword indicates that a value may change between different accesses,
// even if it does not appear to be modified. Must use 'volatile' if interrupts
// access this data

// Telemetry on the last packet
volatile int8_t radio_last_rssi = -128;
volatile uint8_t radio_last_lqi = 0;
volatile int8_t radio_last_freqest = 0;
volatile __xdata uint32_t radio_cs_count = 0;

volatile __bit rf_rx_underway, rf_rx_complete;

volatile __xdata rf_buffer_t rf_rx_buffer;
volatile __xdata rf_buffer_t rf_tx_buffer;

__xdata uint16_t last_rx_ticks;

__xdata uint32_t radio_packets_sent;
__xdata uint32_t radio_packets_good;
__xdata uint32_t radio_packets_rejected_checksum;
__xdata uint32_t radio_packets_rejected_authent;
__xdata uint32_t radio_packets_rejected_other;


// Controls whether the rftxrx ISR is transmitting or receiving.
// The ISR sets this bit, for example in transmit we set it high
// then after we are done transsmitting the rf_isr sets this bit low
volatile __bit rf_mode_tx = 0;  // controls whether the rftxrx ISR is transmitting or receiving

/**
 * @brief Apply appropriate register settings based on rx/tx mode
 *
 * @param[in] rx_mode The Receiving radio mode
 * @param[in] tx_mode The Transmitting radio mode
 * @warning Ensure receive and transmit modes are identical
 */
void radio_set_modes(uint8_t rx_mode, uint8_t tx_mode) {
    // Get register sets from board-specific functionality
    radio_mode_rx = rx_mode;
    radio_mode_tx = tx_mode;
}

/**
 * @brief Set up DMA for receiving
 *
 * DMA is used instead of the RFRXTX interrupt, the RF interrupt is still
 * active and monitors for the sync words and end of packets
 * Packet length settings are configured in radio_set_modes()
 *
 * @pre DMA must be initialized
 * @sideeffects Data is transferred to the RX buffer
 */
static void radio_rx_setup_dma(void) {

    // Make sure the trigger interrupts are disabled (probably unnecessary)
    RFTXRXIE = 0;


    // The rf_rx channel transfers data from the radio data register RFD
    // to the rx buffer in memory once a packet starts.
    // We know a packet starts based on the RFRXTX interrupt
    dma_configure_transfer(

        // We are configuring the Radio RX DMA channel
        dma_channel_rf,

        // Data is transfered from the RFD register (radio in)
        &X_RFD,

        // Data is transfered to the RX Buffer
        // Use data[0] or data[1] depending on fixed or variable packet mode
        // If fixed packet mode the length was automatically inserted and
        // We want to process data from data[1]. Because data[0] contains the length
        // byte that was pre-inserted.
        // In variable packet mode, we don't automatically insert the packet length
        // The packet length is the first byte in the payload data i.e. data[0]
        &rf_rx_buffer.pkt_bytes[(PKTCTRL0 & PKTCTRL0_LENGTH_CONFIG_VARIABLE) ? 0 : 1],

        // RFD produces one byte at a time
        DMA_WORDSIZE_8_BIT |

        // Single transfer (don't automatically rearm, we rearm manually
        // after each call to radio_listen())
        DMA_TMODE_SINGLE |

        // Trigger on the radio RFRXTX interrupt (send each byte
        // when the radio is ready for data)
        // See DMA Trig table:  '19' 'RADIO' 'Radio RF packet byte received/transmit'
        // I assume here that RADIO is mapped to the RFRXTX interrupt
        DMA_TRIG_RADIO,

        // Do not increment the pointer to RFD
        DMA_SRCINC_ZERO |

        // After each byte, increment our offset in the radio RX buffer
        // by one
        DMA_DESTINC_ONE |

        // No interrupt is generated when the transfer is done
        DMA_IRQMASK_DISABLE |

        // In variable length modes, use all 8 bits of the first
        // byte as the length (rather than just 7)
        DMA_M8_ALL8 |

        // Normal memory priority means this channel round-robins
        // with the CPU and other channels when they compete for
        // memory access
        DMA_PRIORITY_NORMAL);
}

/**
 * @brief Set up DMA for transmitting
 *
 * DMA is used instead of the 'RFRXTX' interrupt, the 'RF' interrupt is still
 * active and monitors for the sync words and end of packets
 *
 * The CC1110 does not have a traditional TX FIFO. However, we mimic the behavior of a FIFO
 * by setting up a memory area in RAM on the SoC to store the contents of the packet.
 *
 * @pre DMA must be initialized
 */
static void radio_tx_setup_dma(void) {

    // Make sure the trigger interrupts are disabled (probably unnecessary)
    RFTXRXIE = 0;

    // The rf_tx channel transfers data from the tx buffer in memory
    // to the radio data register when a packet is transmitted
    dma_configure_transfer(
        // We are configuring the Radio TX DMA channel
        dma_channel_rf,
        // Data is transfered from the TX Buffer.
        // Data is transfered to the RX Buffer.
        // Use data[0] or data[1] depending on fixed or variable packet mode
        // If fixed packet mode the length was automatically inserted and
        // We want to process data from data[1]. Because data[0] contains the length
        // byte that was pre-inserted
        // In variable packet mode, we don't automatically insert the packet length
        // the packet length is the first byte in the payload data i.e. data[0]
        &rf_tx_buffer.pkt_bytes[(PKTCTRL0 & PKTCTRL0_LENGTH_CONFIG_VARIABLE) ? 0 : 1],

        // Data is transfered to the RFD register (radio out)
        &X_RFD,

        // RFD accepts one byte at a time
        DMA_WORDSIZE_8_BIT |

        // Single transfer (don't automatically rearm, we rearm manually
        // when a packet is ready)
        DMA_TMODE_SINGLE |

        // Trigger on the radio RFRXTX interrupt (send each byte
        // when the radio is ready for data)
        DMA_TRIG_RADIO,

        // After each byte, increment our offset in the radio TX buffer
        // by one
        DMA_SRCINC_ONE |

        // Do not increment the pointer to RFD
        DMA_DESTINC_ZERO |

        // No interrupt is generated when the transfer is done
        DMA_IRQMASK_DISABLE |

        // In variable length modes, use all 8 bits of the first
        // byte as the length (rather than just 7)
        DMA_M8_ALL8 |

        // Normal memory priority means this channel round-robins
        // with the CPU and other channels when they compete for
        // memory access
        DMA_PRIORITY_NORMAL);
}

/**
 * @brief Configure the length settings for DMA
 *
 * @pre DMA must be initialized
 */
static void radio_setup_dma_len(void) {
    // Configure the length settings for DMA
    if (PKTCTRL0 & PKTCTRL0_LENGTH_CONFIG_VARIABLE) {
        dma_configure_length(
            dma_channel_rf,
            // Transfer the length byte + N bytes
            DMA_VLEN_VARIABLE_PLUS_1,
            // Maximum transfer size is the size of the buffer
            RF_BUFFER_SIZE_BYTES);
    } else {
        dma_configure_length(
            dma_channel_rf,
            // Transfer the length byte + N bytes
            DMA_VLEN_FIXED_USE_LEN,
            // Fixed transfer size
            PKTLEN);
    }
}

/**
 * @brief Initialize radio registers
 */
void radio_init(void) {
    // Reset the radio to the idle state
    RFST = RFST_SIDLE;

    last_rx_ticks = 0;
    // TODO: default channel
    radio_packets_sent = 0;
    radio_packets_good = 0;
    radio_packets_rejected_checksum = 0;
    radio_packets_rejected_authent = 0;
    radio_packets_rejected_other = 0;
    // TODO: default channel?
    radio_set_modes(RADIO_MODE_DEFAULT_RX, RADIO_MODE_DEFAULT_TX);
}

/**
 * @brief Get message
 * Checks CRC, MAC, and TS, in that order
 * If any are invalid, the packet is treated as invalid
 *
 * @param[out] cmd The place to put the incoming command.
 * @sideeffect Telemetry field radio_packets will be updated
 * @return Length of command_t cmd (commands.h)
 */
uint8_t radio_get_message(__xdata command_t *cmd) {
    uint8_t rf_pkt_length;
    uint8_t msg_length;
    __xdata rf_command_footer_t *footer;

    // If there is no packet ready just return 0
    if (!rf_rx_complete) {
        return 0;
    }

    // Make sure the packet is long enough to be parseable
    // If len is too long then the packet would have already been auto discarded by CC1110 RF hardware
    rf_pkt_length = rf_rx_buffer.header.len;
    // RF packet must have all fields except data which can be 0 bytes
    if (rf_pkt_length < RF_PACKET_MIN_LEN) {
        // If not just drop it
        radio_packets_rejected_other++;
        rf_rx_complete = 0;
        radio_listen();
        return 0;
    }

    footer = (__xdata rf_command_footer_t *) &rf_rx_buffer.pkt_bytes[
        sizeof(rf_rx_buffer.header.len) + rf_pkt_length - sizeof(rf_command_footer_t)];

    // check crc
    // TODO: handle fixed length mode
    if (crc16(rf_rx_buffer.pkt_bytes, sizeof(rf_rx_buffer.header.len) + rf_pkt_length - sizeof(footer->crc)) != footer->crc) {
        radio_packets_rejected_checksum++;
        rf_rx_complete = 0;
        radio_listen();
        return 0;
    }

    #ifndef BOOTLOADER

    // check mac and ts
    if (check_packet_authent(&rf_rx_buffer) == 0) {
        radio_packets_rejected_authent++;
        rf_rx_complete = 0;
        radio_listen();
        return 0;
    }

    #endif

    // This is a good packet - update the last received time
    last_rx_ticks = 0;

    // len is now command_t len
    msg_length = rf_pkt_length - sizeof(rf_command_footer_t);

    // Now copy the message to the cmd struct.
    memcpyx((__xdata void *) cmd,
            (__xdata void *) rf_rx_buffer.header.cmd,
            msg_length);

    rf_rx_complete = 0;
    radio_packets_good++;
    return msg_length;
}

/**
 * @brief RF ISR control routines to deal with transmitting and receiving a packet
 *
 * There are 8 different events that can generate
 * an RF interrupt request, we use some of them here
 *
 * @warning Changing RF interrupt settings (RFIF) will change sending packets functionality
 */
void rf_isr(void)  __interrupt (RF_VECTOR) __using (1) {

    // If the corresponding mask bit is set in the RFIM
    // register, the CPU interrupt flag S1CON.RFIF
    // will also be asserted in addition to the interrupt
    // flag in RFIF. If IEN2.RFIE=1 when
    // S1CON.RFIF is asserted, and interrupt
    // request will be generated.
    // So we get a CPU interrupt and want to clear it
    // by setting S1CON = 0, and then deal with the RFIF
    // interrupts which were set via RFIM
    S1CON = 0;  // Clear RFIF_1 and RFIF_2
    if (RFIF & RFIF_IM_TXUNF) {
        rf_mode_tx = 0;
        // TODO: error flag
    }

    // If a packet was received or transmitted and rf_mode_tx =1, set it
    // to 0 via an interrupt
    if (RFIF & RFIF_IM_DONE && rf_mode_tx) {
        rf_mode_tx = 0;
    }

    // If a packet was received/ transmitted and rf_mode_tx = 0, set it to 1
    if (RFIF & RFIF_IM_DONE && !rf_mode_tx) {

        // We have correctly received a packet
        rf_rx_complete = 1;

        // Disable the packet receieved/ transmitted interrupt
        RFIF = (uint8_t)~RFIF_IM_DONE;

        // Update telemetry fields with physical layer packet information
        radio_last_rssi = *((int8_t *) &RSSI);
        radio_last_lqi = LQI;
        radio_last_freqest = *((int8_t *) &FREQEST);
    }

    // RX SFD - Packet reception begun (sync word detected)
    if (RFIF & RFIF_IM_SFD && !rf_mode_tx) {
        // RX SFD - Packet reception begun (sync word detected)
        rf_rx_underway = 1;
    }

    // Carrier sense went above threshold so we record this happened
    if (RFIF & RFIF_IM_CS) {
        radio_cs_count++;
    }

    // RF Interrupt Flags is RFIF
    // Here we clear all pending RF interrupts
    RFIF = 0;

    // Interrupts flags have been dealt with and we can move on
}

#pragma disable_warning 158
/**
 * @brief Set the appropriate rx interrupt masks and listen to the channel
 */
void radio_listen(void) {
    // Drop to the IDLE state
    // If we hit any error states (like underflow/overflow)
    // this will also clear that error

    // Disable RF interrupt requests
    IEN2 &= ~IEN2_RFIE;
    RFST = RFST_SIDLE;

    if (radio_disable_rx) {
        return;
    }

    #if BOARD_HAS_RX_HOOK == 1
    board_pre_rx();
    #endif

    /*
         alea - commenting out, not required for the same rx and tx radio_set_modes,
         currently call before main loop and when parameters are reset, otherwise
         parameters are set by controller (OBC)
    */
    // board_apply_radio_settings(radio_mode_rx);

    // Abort any ongoing DMA transaction (RX or TX) on our channel
    dma_abort(dma_channel_rf);

    radio_rx_setup_dma();
    radio_setup_dma_len();

    // Clear the TX/RX interrupt flag
    RFTXRXIF = 0;

    // Arm the DMA transfer
    dma_arm(dma_channel_rf);

    // Clear all RF IRQ flags
    RFIF = 0;

    // Set the interrupt mask to interrupt
    // on the following conditions
    RFIM = RFIM_IM_DONE |  // Packet received or transmitted
           RFIM_IM_SFD |   // Start of frame (sync word) detected
           RFIM_IM_CS;     // Carrier sense (for telemetry)
    // Enable general RF interrupt requests
    IEN2 |= IEN2_RFIE;

    // Initialize interrupt flags used in telemetry fields
    // These are typed as volatile because the rf_isr may read/write to them
    rf_mode_tx = 0;
    rf_rx_underway = 0;
    rf_rx_complete = 0;

    #ifndef BOOTLOADER
    // Set timer 1 to watch for the frame start and capture
    // precise timing for ranging
    timers_watch_for_RF();
    #endif

    // Command strobe for receive mode
    RFST = RFST_SRX;
}


/**
 * @brief Configure radio to send a packet and send it
 *
 * @param[in] cmd The cmd to send
 * @param[in] precise_timing TODO
 */
void radio_send_packet(const __xdata command_t* cmd, uint8_t len, __bit precise_timing) {
    __xdata rf_command_footer_t *footer;
    uint8_t rf_pkt_len = len + sizeof(rf_command_footer_t);

    #ifndef BOOTLOADER
    if (precise_timing) {
        // Enable the timer interrupt now. The interrupt will send STX
        // after the specified number of counts.
        // This assumes we can get the reply ready before the specified
        // number of ticks occur. Otherwise we will experience an underflow.
        // The default is two 1ms ticks which is plenty of time.
        timers_trigger_for_RF();
    }
    #else
    precise_timing;  // To avoid compiler warnings about this variable
    #endif

    // Make sure the packet isn't too big
    // The RF packet adds a footer.
    if (len > sizeof(command_t)) {
        // TODO logging?
        // Drop the packet and return to caller if the length is too large
        return;
    }

    // Drop to the IDLE state
    // If we hit any error states (like underflow/overflow)
    // this will also clear that error

    // Disable RF interrupt requests
    IEN2 &= ~IEN2_RFIE;
    RFST = RFST_SIDLE;

    /*
         alea - commenting out, not required for the same rx and tx radio_set_modes,
         currently call before main loop and when parameters are reset, otherwise
         parameters are set by controller (OBC)
    */
    // board_apply_radio_settings(radio_mode_tx);

    // Abort any ongoing DMA transaction (RX or TX) on our channel
    dma_abort(dma_channel_rf);

    radio_tx_setup_dma();
    radio_setup_dma_len();

    // Clear the tx buffer just to be safe
    // TODO: not needed?
    memsetx(rf_tx_buffer.pkt_bytes, 0, sizeof(rf_tx_buffer.pkt_bytes));
    // First, copy in the command to the RF buffer
    memcpyx((__xdata void *) rf_tx_buffer.header.cmd, (__xdata void *) cmd, len);

    // Set length
    rf_tx_buffer.header.len = rf_pkt_len;

    footer = (__xdata rf_command_footer_t *)(&rf_tx_buffer.pkt_bytes[sizeof(rf_tx_buffer.header.len) + len]);

    #ifndef BOOTLOADER

    // compute mac and ts and add to packet
    add_packet_authent(&rf_tx_buffer);
    // TODO: log if return is 0

    #endif


    // compute crc over all fields including length field
    // TODO: handle fixed length packets
    footer->crc = crc16(rf_tx_buffer.pkt_bytes, sizeof(rf_tx_buffer.header.len) + rf_pkt_len - sizeof(footer->crc));


    #if BOARD_HAS_TX_HOOK == 1
    board_pre_tx();
    #endif

    // If too few bytes are written to the
    // RFD register compared to what the radio expect there will be an underflow.
    // We set the RF ISR to interrupt if there is an underflow
    // or when its done transmitting
    RFIM = RFIM_IM_TXUNF | // TX Underflow
           RFIM_IM_DONE;

    // For an interrupt request to be generated when
    // TCON.RFTXRXIF is asserted,
    // IEN0.RFTXRXIE must be 1.
    // We don't want to generate an interrupt this way so we
    // set it to 0. We use the RFRXTX interrupt for DMA trigger only

    // Ensure that RFTXRX interrupt is not used here
    RFTXRXIF = 0;
    RFTXRXIE = 0;  // ensure that RFTXRX interrupt is not used
    RFIF = 0;
    IEN2 |= IEN2_RFIE;

    dma_arm(dma_channel_rf);

    // Start transmitting now if we aren't using the timer interrupt
    // to control the transmit time
    rf_mode_tx = 1;
    #ifdef BOOTLOADER
    RFST = RFST_STX;
    #else
    if (!precise_timing) {
        RFST = RFST_STX;
    }
    #endif

    while(rf_mode_tx); // Block until TX complete

    radio_listen();
    radio_packets_sent++;
}
