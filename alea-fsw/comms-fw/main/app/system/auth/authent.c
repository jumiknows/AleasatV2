#include <stdint.h>

#include "authent.h"
#include "radio.h"
#include "aes.h"
#include "flash_vars.h"
#include "routing.h"
#include "stringx.h"
#include "timers.h"
#include "board_defaults.h"
#include "radio_commands.h"

__xdata uint8_t aro_key[AES_KEY_SIZE] = {0};
__xdata uint8_t authent_enable = AUTHENTICATION_ENABLED_ON_BOOT;

static __xdata uint8_t* key_table[NUM_DESTS] = {
    0,
    0,
    (__xdata uint8_t*) &flash_keys[0],
    (__xdata uint8_t*) &flash_keys[1 * AES_KEY_SIZE],
    aro_key,
    0
};

static __xdata uint8_t cbc_mac_result[AES_BLOCK_SIZE];


/**
 * @brief Checks CBC-MAC and timestamp authentication in packet footer
 *
 * @pre aes_init
 * @param pkt RF packet received
 * @return 1 if packet has correct authentication, 0 if packet is bad or has bad authentication
 */
__bit check_packet_authent(__xdata rf_buffer_t* pkt) {
    uint8_t pkt_len;
    dest_t src;
    __xdata uint8_t *key;
    __xdata rf_command_footer_t *footer;

    pkt_len = pkt->header.len;

    footer = (__xdata rf_command_footer_t *) &pkt->pkt_bytes[
        sizeof(pkt->header.len) + pkt_len - sizeof(rf_command_footer_t)];

    src = get_dest_from_hwid(pkt->header.cmd.header.src_hwid);
    if (src == DEST_INVALID) {
        return 0;
    }

    // if ARO then only allowed dest is OBC
    // TODO: refactor into input_handlers
    if (src == DEST_ARO) {
        if (get_dest_from_hwid(pkt->header.cmd.header.dest_hwid) != DEST_OBC) {
            return 0;
        }
    }

    key = key_table[src];
    if (key == 0) {
        return 0;
    }

    // Don't check auth if it's not enabled
    if (authent_enable == 0) {
        return 1;
    }

    // check CBC-MAC
    aes_compute_cbc_mac(pkt->pkt_bytes,
        sizeof(pkt->header.len) + pkt_len - sizeof(footer->crc) - sizeof(footer->mac),
        key,
        cbc_mac_result);
    if (memcmpx_ct(cbc_mac_result, footer->mac, sizeof(footer->mac)) != 0) {
        // CBC-MAC differs
        return 0;
    }

    // only check ts if time is set on board
    if (rtc_set == 1) {
        if (timers_get_seconds() > footer->timestamp + TIMESTAMP_VALID_WINDOW_SECONDS) {
            // packet timestamp is too old
            return 0;
        }
    }

    return 1;
}


/**
 * @brief Adds CBC-MAC and timestamp authentication to packet footer
 * 
 * @pre aes_init
 * @param pkt RF packet to be sent, must already contain length, message, and TS
 * @return 1 if authent added to packet, 0 if packet is bad or shouldn't have authent added
 */
__bit add_packet_authent(__xdata rf_buffer_t* pkt) {
    uint8_t pkt_len;
    dest_t dest;
    __xdata uint8_t *key;
    __xdata rf_command_footer_t *footer;

    pkt_len = pkt->header.len;

    footer = (__xdata rf_command_footer_t *) &pkt->pkt_bytes[
        sizeof(pkt->header.len) + pkt_len - sizeof(rf_command_footer_t)];

    dest = get_dest_from_hwid(pkt->header.cmd.header.dest_hwid);
    if (dest == DEST_INVALID) {
        return 0;
    }

    key = key_table[dest];
    if (key == 0) {
        return 0;
    }

    // Fill mac and ts with 0's if it's not enabled
    if (authent_enable == 0) {
        memsetx((__xdata void *) footer->timestamp, 0, sizeof(footer->timestamp) + sizeof(footer->mac));
        return 1;
    }

    footer->timestamp = rtc_set ? timers_get_seconds() : 0;

    aes_compute_cbc_mac(pkt->pkt_bytes,
        sizeof(pkt->header.len) + pkt_len - sizeof(footer->crc) - sizeof(footer->mac),
        key,
        footer->mac);

    return 1;
}
