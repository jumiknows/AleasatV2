/**
 * @file comms_cc1110.h
 * @brief Driver for interacting with OpenLST part of Comms MCU (TI CC1110)
 * @author Michael Lin
 * @date Feb 19, 2021
 *
 * @see https://gitlab.com/alea-2020/communications/comms-firmware-openlst-1/-/tree/master
 * @see https://www.ti.com/lit/ds/symlink/cc1110-cc1111.pdf
 */

#ifndef COMMS_CC1110_H_
#define COMMS_CC1110_H_

typedef enum { common_msg_ack = 0x10, common_msg_nack = 0xff, common_msg_ascii = 0x11 } comms_lst_cmd_e;

typedef enum {
    // Older versions of the bootloader used
    // separate command codes for serial connections
    // and radio connections.
    // Both are included here for compatiblity
    bootloader_msg_ping       = 0x00,
    bootloader_msg_erase      = 0x0c,
    bootloader_msg_write_page = 0x02,
    bootloader_msg_ack        = 0x01,
    bootloader_msg_nack       = 0x0f
} bootloader_msg_no;

typedef enum {
    radio_msg_reboot       = 0x12,
    radio_msg_get_time     = 0x13,
    radio_msg_set_time     = 0x14,
    radio_msg_ranging      = 0x15,
    radio_msg_ranging_ack  = 0x16,
    radio_msg_get_telem    = 0x17,
    radio_msg_telem        = 0x18,
    radio_msg_get_callsign = 0x19,
    radio_msg_set_callsign = 0x1a,
    radio_msg_callsign     = 0x1b
} radio_msg_no;


#endif /* COMMS_CC1110_H_ */
