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

#ifndef _RADIO_H
#define _RADIO_H


#include <stdint.h>
#include <cc1110.h>
#include "commands.h"
#include "hwid.h"

/**
 * @brief struct to hold the radio configuration.
 */
typedef struct {
    uint8_t pktctrl1;    // Packet Automation Control
    uint8_t pktctrl0;    // Packet Automation Control
    uint8_t channr;      // Channel Number
    uint8_t fsctrl1;     // Frequency Synthesizer Control
    uint8_t fsctrl0;     // Frequency Synthesizer Control
    uint8_t freq2;       // Frequency Control Word, High Byte
    uint8_t freq1;       // Frequency Control Word, Middle Byte
    uint8_t freq0;       // Frequency Control Word, Low Byte
    uint8_t sync0;
    uint8_t sync1;
    uint8_t mdmcfg4;     // Modem configuration
    uint8_t mdmcfg3;     // Modem Configuration
    uint8_t mdmcfg2;     // Modem Configuration
    uint8_t mdmcfg1;     // Modem Configuration
    uint8_t mdmcfg0;     // Modem Configuration
    uint8_t deviatn;     // Modem Deviation Setting
    uint8_t mcsm2;       // Main Radio Control State Machine Configuration
    uint8_t mcsm1;       // Main Radio Control State Machine Configuration
    uint8_t mcsm0;       // Main Radio Control State Machine Configuration
    uint8_t foccfg;      // Frequency Offset Compensation Configuration
    uint8_t bscfg;       // Bit Synchronization Configuration
    uint8_t agcctrl2;    // AGC Control
    uint8_t agcctrl1;    // AGC Control
    uint8_t agcctrl0;    // AGC Control
    uint8_t frend1;      // Front End RX Configuration
    uint8_t frend0;      // Front End TX Configuration
    uint8_t fscal3;      // Frequency Synthesizer Calibration
    uint8_t fscal2;      // Frequency Synthesizer Calibration
    uint8_t fscal1;      // Frequency Synthesizer Calibration
    uint8_t fscal0;      // Frequency Synthesizer Calibration
    uint8_t test2;       // Various Test Settings
    uint8_t test1;       // Various Test Settings
    uint8_t test0;       // Various Test Settings
    uint8_t pa_table0;   // PA Power Setting 0
    uint8_t pa_table0_reduced;   // PA Power Setting 0 for reduced power mode
    uint8_t pktlen;      // Packet length (max possible for variable-length modes)
} radio_settings_t;

/**
 * @brief Flag signalling a valid radio mode is chosen
 */
#define RADIO_MODE_OK      0

/**
 * @brief Flag signalling an invalid radio mode is chosen
 */
#define RADIO_MODE_INVALID 1

// For radio_send_packet, support controlled response
// timing for ranging. "Precise" replies have a delay
// before transmission (the default is 2ms, but this depends
// on the timer and clock settings) that is fixed and well
// controlled. "Now" packets have no delay but receipt->reply
// times can be variable.
#define RF_TIMING_NOW     0
#define RF_TIMING_PRECISE 1


#define MAC_FIELD_LEN 16


/**
 * @brief Convenience struct for accessing rf_buffer_t
 */
typedef struct rf_command_header {
    uint8_t len;
    command_t cmd;
} rf_command_header_t;

/**
 * @brief Footer is appended to end of command_t cmd data types and used in RF packets only
 */
typedef struct rf_command_footer {
    uint32_t timestamp;
    uint8_t mac[MAC_FIELD_LEN];
    uint16_t crc;
} rf_command_footer_t;

/**
 * @brief Maximumn RF packet size including length byte
 */
#define RF_BUFFER_SIZE_BYTES 256
#define RF_PACKET_MAX_LEN 255
#define RF_PACKET_MIN_LEN (sizeof(command_header_t) + sizeof(rf_command_footer_t))

/**
 * @brief Store RF packets in a correctly sized buffer for read/write access
 * Includes length byte
 *
 * Accessing RF footer requires manual indexing of array with length
 */
typedef union {
    uint8_t pkt_bytes[RF_BUFFER_SIZE_BYTES];
    rf_command_header_t header;
} rf_buffer_t;


void rf_isr(void)  __interrupt (RF_VECTOR) __using (1);
void radio_set_modes(uint8_t rx_mode, uint8_t tx_mode);
uint8_t radio_get_message(__xdata command_t *cmd);
void radio_init(void);
void radio_listen(void);
void radio_send_packet(const __xdata command_t* cmd, uint8_t len, __bit precise_timing);

// Tell the compiler to declare these telemetry fields
// but don't intialize them yet.
// Use volatile because ISR may access and change them.
// Use extern to make available globally
extern uint8_t radio_mode_tx;
extern uint8_t radio_mode_rx;

extern __bit radio_disable_rx;

extern volatile int8_t radio_last_rssi;
extern volatile uint8_t radio_last_lqi;
extern volatile int8_t radio_last_freqest;
extern volatile __xdata uint32_t radio_cs_count;

extern __xdata uint16_t last_rx_ticks;
extern __xdata uint32_t radio_packets_sent;
extern __xdata uint32_t radio_packets_good;
extern __xdata uint32_t radio_packets_rejected_checksum;
extern __xdata uint32_t radio_packets_rejected_authent;
extern __xdata uint32_t radio_packets_rejected_other;

#endif
