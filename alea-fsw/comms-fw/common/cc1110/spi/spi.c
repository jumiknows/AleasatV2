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

// SPI setup and interrupt handler routines
#ifdef BOOTLOADER
#pragma codeseg APP_UPDATER
#endif
#include <cc1110.h>
#include <string.h>
#include <stdint.h>
#include "cc1110_regs.h"
#include "board_defaults.h"
#include "flash_vars.h"
#include "hwid.h"
#include "uart.h"
#include "spi.h"
#include "stringx.h"

volatile __data uint32_t spi_rx_count;

static esp_state_t __data rx_esp_state;
static uint8_t __data rx_buffer_ready[SPI_RX_BUFFERS];
static uint8_t __data rx_buffer_len[SPI_RX_BUFFERS];
static uint8_t __data rx_active_buffer;
static uint8_t __data rx_buffer_offset;
static uint8_t __xdata rx_buffer[SPI_RX_BUFFERS][LOCAL_PACKET_MAX_LEN];
// TODO: do we need multiple buffers?

#if SPI_ENABLED == 1
/**
 * @brief Initialize spi settings.
 */
void spi_init(void) {
    uint8_t b;

    // Initialize the receive counter
    spi_rx_count = 0;

    // USART0 set to operate as SPI slave
    U0CSR   = 0x20;

    // USART0 I/O location set to alternative 2 location
    PERCFG |= 0x01;

    // Selected port 1 function pins set to peripheral function pins
    P1SEL  |= 0x3C;

    // Port 1, pin 5 direction set to output
    P1DIR  |= (1<<5);

    // Set baud rate to 187500 baud (not actually needed for SPI slave)
    // Set baud rate mantissa
    // U0BAUD  = 199;
    // Set to CPOL0, CPHA0, MSBit first, baud rate exponent=12
    U0GCR   = 0x20 | 0x0C;

    // Clear any rx buffers
    for (b = 0; b < SPI_RX_BUFFERS; b++) {
        rx_buffer_ready[b] = 0;
    }

    // Set SPI RX interrupts to level 1 priority, same as UART
    // (see http://www.ti.com/lit/ds/symlink/cc1110-cc1111.pdf pg 69)
    IP0    |= IP0_IPG2;
    IP1    &= ~(IP1_IPG2);

    // Disable USART0 TX interrupt
    IEN2   &= ~IEN2_UTX0IE;
    // Clear USART0 TX interrupt flag
    UTX0IF  = 0;

    // Enable USART0 RX interrupt
    URX0IE  = 1;
    // Clear USART0 RX interrupt flag
    URX0IF  = 0;

    // Flushes unit
    U0UCR   = 0x80;

    // Use Timer 2 as SPI TX timeout timer
    // Can't use Timer 3/4 because precsaler * timer size isn't large enough
    // Timer 2 is 8 bits in size and has an 18 bit prescaler, it counts down and raises TEX when it reaches 0x00
    // TICKSPD is always at max frequency (27 MHz) so if we want ex. 20ms before timer expires,
    // We want prescaler * 255 to equal 2,700,000
    // One close option is T2PR = 33 (0b00100001) and TIP = 64 (aka set to 0b00)
    // See datasheet 12.7

    // configure 20ms timer
    T2CT    = 0x00;
    T2CTL   = 0b00000000;
    T2PR    = 0b00100001;
}

#ifndef BOOTLOADER
/**
 * @brief Check if all SPI buffers are empty
 *
 * @return If all buffers are empty or not
 */
__bit spi_buffers_empty(void) {
    __bit ret;
    uint8_t i;

    ret = 1;
    for (i = 0; i < SPI_RX_BUFFERS; i++) {
        if (rx_buffer_ready[i] == 1) {
            ret = 0;
            break;
        }
    }
    return ret;
}
#endif /* BOOTLOADER */

/**
 * @brief Get SPI message by checking the SPI_RX_BUFFERS
 *
 * @param[out] buf The buffer where the command will be copied to.
 * @return The length of the output buffer.
 */
uint8_t spi_get_message(__xdata uint8_t *buf) {
    // If there is a completed message in a buffer,
    // this function will copy that message to buf
    // and return the length in bytes.
    // If no messages are ready, buf is left unchanged
    // and 0 is returned.
    uint8_t i;
    uint8_t len;
    for (i = 0; i < SPI_RX_BUFFERS; i++) {
        if (rx_buffer_ready[i]) {
            // Copy the message to the output buffer
            len = rx_buffer_len[i];
            memcpyx(buf, rx_buffer[i], len);
            // Release the buffer
            rx_buffer_ready[i] = 0;
            return len;
        }
    }
    // No finished buffers found
    return 0;
}

/**
 * @brief Put single char into the SPI buffer, not used anymore
 *
 * @param c The char to send
 */
inline void spi_put(char c) {
    c;
    // doesn't work well even though TI recommends this (duplicate bytes, bits shifted, too slow)
    // U0CSR &= ~0x04;  // clear flag
    // U0DBUF = c;
    // while (!(U0CSR & 0x04));  // wait until char is accepted
    // U0CSR &= ~0x04;  // clear flag

    // Also doesn't work well (sometimes misses an edge on SPI CLK or too slow to put byte in buffer?)
    // while (!UTX0IF);  // UTX0IF is asserted at start of SPI CLK edge
    // U0DBUF = c;
    // UTX0IF = 0;  // clear the TX interrupt flag

    // The most reliable out of the bunch by far
    // UTX0IF = 0;  // clear the TX interrupt flag because it might be set from extra SPI operations from OBC
    // U0DBUF = c;
    // while (!UTX0IF);  // wait for byte to begin transmission
    // UTX0IF = 0;  // clear the TX interrupt flag
}

/**
 * @brief Busy waits until U0CSR.ACTIVE equals given value
 *
 * @param b Busy waits until U0CSR.ACTIVE equals this value
 */
void spi_cs_wait_for(__bit b) {
    volatile __bit v;
    while(1) {
        v = U0CSR & 0x01;
        if (v == b) {
            break;
        }
    }
}

inline void spi_tx_timer_start() {
    // approximately 20ms from values set in spi_init
    T2CT   = 0xFF;
    T2CTL &= 0b10111111;  // Reset TEX flag
}

inline void spi_tx_timer_start_inter() {
    // set timeout period for when at least one byte has been read out
    // 20ms * 3 / 255 is around 235us
    T2CT   = 0x03;
    T2CTL &= 0b10111111;  // Reset TEX flag
}

inline __bit spi_tx_timer_is_exp() {
    return T2CTL & 0b01000000;
}

inline void spi_tx_timer_reset() {
    T2CT   = 0x00;
    T2CTL &= 0b10111111;  // Reset TEX flag
}

// last byte is len which will be filled in
static __data uint8_t local_pkt_header[3] = {ESP_START_BYTE_0, ESP_START_BYTE_1, 0};

/**
 * @brief Send msg that is len long over SPI.
 *
 * @param msg The message to send
 * @param len The length of the message
 */
void spi_send_message(const __xdata uint8_t *msg, uint8_t len) {
    __data uint8_t i;

    local_pkt_header[2] = len;

    spi_cs_wait_for(0);  // wait for SPI_CS to be released
    URX0IE = 0; // disable RX interrupt
    spi_tx_timer_start();
    P0_0 = 0; // trigger falling edge on COMMS interrupt pin

    for(i = 0; i < 3; ++i) {
        UTX0IF = 0;
        U0DBUF = local_pkt_header[i];
        while (1) {
            if (UTX0IF) {
                spi_tx_timer_start_inter();
                break;
            }
            if (spi_tx_timer_is_exp()) {
                goto SPI_END;
            }
        }
    }

    while (len--) {
        UTX0IF = 0;
        U0DBUF = *(msg++);
        while (1) {
            if (UTX0IF) {
                spi_tx_timer_start_inter();
                break;
            }
            if (spi_tx_timer_is_exp()) {
                goto SPI_END;
            }
        }
    }

SPI_END:
    U0DBUF = 0;  // set SPI buffer to 0 so remaining bytes on wire is just all zeros
    spi_tx_timer_reset();
    P0_0 = 1; // set COMMS interrupt pin back to high
    spi_cs_wait_for(0);  // wait for SPI_CS to be released. TODO: is this required?
    U0UCR  = 0x80;  // flush USART after each message
    URX0IF = 0;  // clear the RX interrupt flag
    URX0IE = 1; // enable RX interrupt
    return;

// alternate version to handle case where OBC asserts SPI CS but never starts SCLK
// SPI_END:
//     U0DBUF = 0;
// reset timer
//     P0_0 = 1;
//     U0UCR  = 0x80;
//     URX0IF = 0;
//     URX0IE = 1;
//     return;
}

/**
 * @brief Command buffer for printing.
 */
static __xdata command_t print_buf;

/**
 * @brief Send message to destination over SPI
 *
 * @param dest
 * @param cmd
 * @param data
 * @param data_len
 */
void send_command_spi(hwid_t dest, radio_msg_no_t cmd, const uint8_t *data, uint8_t data_len) {
    __data uint8_t len = sizeof(print_buf.header) + data_len;

    print_buf.header.seqnum_and_resp = 0;
    print_buf.header.src_hwid = hwid_flash;
    print_buf.header.dest_hwid = dest;
    print_buf.header.cmd_num = cmd;
    memcpyx((__xdata uint8_t *) print_buf.data, (__xdata uint8_t *) data, data_len);
    spi_send_message((__xdata void *) &print_buf, len);
}


/**
 * @brief SPI ISR
 */
void spi_rx_isr() __interrupt (URX0_VECTOR) __using (3) {
    uint8_t c;
    URX0IF = 0;  // clear RX interrupt flag
    c = U0DBUF;
    switch (rx_esp_state) {
        case wait_for_start0:
            // Waiting for a packet to start
            if (c == ESP_START_BYTE_0) {
                rx_esp_state = wait_for_start1;
            }
            break;

        case wait_for_start1:
            if (c == ESP_START_BYTE_1) {
                rx_esp_state = wait_for_length;
            } else if (c == ESP_START_BYTE_0) {
                rx_esp_state = wait_for_start1;
            }
            // TODO: else go to wait_for_start0?
            break;

        case wait_for_length:
            if (c > LOCAL_PACKET_MAX_LEN || c < 1) {
                // Skip this packet if it is too long to handle
                rx_esp_state = wait_for_start1;
            } else {
                // Find a free buffer
                rx_active_buffer = 0;
                while ((rx_active_buffer < SPI_RX_BUFFERS) && rx_buffer_ready[rx_active_buffer]) {
                    rx_active_buffer++;
                }
                if (rx_active_buffer >= SPI_RX_BUFFERS) {
                    // No free buffers, just skip this packet
                    rx_active_buffer = 0;
                    rx_esp_state = wait_for_start0;
                } else {
                    rx_buffer_len[rx_active_buffer] = c;
                    rx_buffer_offset = 0;
                    rx_esp_state = receive_data;
                }
            }
            break;

        case receive_data:
            rx_buffer[rx_active_buffer][rx_buffer_offset++] = c;
            if (rx_buffer_offset == rx_buffer_len[rx_active_buffer]) {
                // This packet is done
                rx_buffer_ready[rx_active_buffer] = 1;
                rx_esp_state = wait_for_start0;
                spi_rx_count++;
            }
            break;
    }
}

#endif
