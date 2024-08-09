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

#include "commands.h"
#include "cc1110_regs.h"
#include "board_defaults.h"
#include "flash_vars.h"
#include "hwid.h"
#include "radio_commands.h"
#include "radio.h"
#include "schedule.h"
#include "stringx.h"
#include "watchdog.h"
#include "authent.h"
#include "telemetry.h"

#ifdef CUSTOM_COMMANDS
uint8_t custom_commands(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply){

    __xdata msg_data_t *cmd_data;
    __xdata msg_data_t *reply_data;
    uint8_t reply_length = sizeof(reply->header);
    cmd_data = (__xdata msg_data_t *) cmd->data;
    reply_data = (__xdata msg_data_t *) reply->data;

    len;  // To avoid compiler warnings about this being unused

    switch (cmd->header.cmd_num) {
        case alea_radio_msg_get_radiotelem:
            update_radiotelem();
            reply->header.cmd_num = alea_radio_msg_radiotelem;
            memcpyx(
                (__xdata void *) &reply_data->radiotelem,
                (__xdata void *) &radiotelem,
                sizeof(reply_data->radiotelem));
            reply_length += sizeof(reply_data->radiotelem);
            break;

        case alea_radio_msg_set_radiotelem:
            reply->header.cmd_num = custom_alea_ack;
            set_radiotelem(&cmd_data->radiotelem);
            break;

        case alea_radio_msg_reset_radiotelem:
            reply->header.cmd_num = custom_alea_ack;
            board_apply_radio_settings(RADIO_MODE_DEFAULT);
            break;

    }
    return reply_length;
}

#endif

uint8_t commands_handle_command(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply) {
    uint8_t reply_length;
    #if RADIO_RANGING_RESPONDER == 1
    uint8_t old_tx_mode;
    #endif
    __xdata msg_data_t *cmd_data;
    __xdata msg_data_t *reply_data;

    __xdata radio_callsign_t *olst_callsign;
    static __xdata radio_callsign_t olst_callsign_rx;

    if (get_resp(cmd->header.seqnum_and_resp) != 0) {
        // Comms doesn't ever expect responses
        return 0;
    }

    // Initialize the reply header
    reply->header.seqnum_and_resp = cmd->header.seqnum_and_resp | 0x8000;  // Set seqnum to be equal and set response bit
    reply->header.dest_hwid = cmd->header.src_hwid;
    reply->header.src_hwid = hwid_flash;

    cmd_data = (__xdata msg_data_t *) cmd->data;
    reply_data = (__xdata msg_data_t *) reply->data;

    // Fallthrough case - use "nack" as the default response
    reply->header.cmd_num = common_msg_nack;
    reply_length = sizeof(reply->header);

    switch (cmd->header.cmd_num) {
        case common_msg_ack:
            reply->header.cmd_num = common_msg_ack;
            break;

        case common_msg_nack:
            reply->header.cmd_num = common_msg_nack;
            break;

        case radio_msg_reboot:
            // Postpone reboot by specified number of seconds
            if (len < sizeof(cmd->header) + sizeof(cmd_data->reboot_postpone)) {
                // If we get a reboot message without the param, reboot immediately
                // (assume the value is 0)
                reply->header.cmd_num = common_msg_ack;
                reboot_scheduled = true;
            } else {
                if (schedule_postpone_reboot(cmd_data->reboot_postpone.postpone_sec) ==
                    SCHEDULE_REBOOT_POSTPONED) {
                    reply->header.cmd_num = common_msg_ack;
                } else {
                    reply->header.cmd_num = common_msg_nack;
                }
            }
            break;

        case radio_msg_get_time:
            if (rtc_set) {
                reply->header.cmd_num = radio_msg_set_time;
                timers_get_time(&reply_data->time);
                reply_length += sizeof(reply_data->time);
            } else {
                // Nack if the time has not been set yet
                reply->header.cmd_num = common_msg_nack;
            }
            break;

        case radio_msg_set_time:
            // TODO: any limits on this?
            reply->header.cmd_num = common_msg_ack;
            timers_set_time(&cmd_data->time);
            break;

        case radio_msg_get_telem:
            reply->header.cmd_num = radio_msg_telem;
            memcpyx(
                (__xdata void *) &reply_data->telemetry,
                (__xdata void *) &telemetry,
                sizeof(reply_data->telemetry));
            reply_length += sizeof(reply_data->telemetry);
            break;

        case radio_msg_set_radio_rx_enabled:
            reply->header.cmd_num = common_msg_ack;
            radio_disable_rx = cmd_data->radio_rx_enabled == 0;
            radio_listen();
            break;

        case radio_msg_set_callsign:
            reply->header.cmd_num = common_msg_ack;
            olst_callsign = (__xdata radio_callsign_t *) cmd->data;

            memsetx((__xdata char *) &olst_callsign_rx, 0, sizeof(*olst_callsign));
            memcpyx((__xdata char *) &olst_callsign_rx,
                (__xdata char *) olst_callsign,
                len - sizeof(command_header_t) > sizeof(*olst_callsign) ? sizeof(*olst_callsign) : len - sizeof(command_header_t));
            break;

        case radio_msg_get_callsign:
            reply->header.cmd_num = radio_msg_callsign;
            olst_callsign = (__xdata radio_callsign_t *) reply->data;

            memcpyx((__xdata char *) olst_callsign,
                (__xdata char *) &olst_callsign_rx,
                sizeof(*olst_callsign));

            reply_length += sizeof(*olst_callsign);
            break;

        case radio_msg_get_authent:
            reply->header.cmd_num = radio_msg_authent_status;
            reply_data->authent_enable = authent_enable;
            reply_length += sizeof(reply_data->authent_enable);
            break;

        case radio_msg_set_authent:
            reply->header.cmd_num = common_msg_ack;
            authent_enable = cmd_data->authent_enable;
            break;

        case radio_msg_get_aro_key:
            reply->header.cmd_num = radio_msg_aro_key;
            memcpyx(reply_data->aes_key, aro_key, sizeof(aro_key));
            reply_length += sizeof(reply_data->aes_key);
            break;

        case radio_msg_set_aro_key:
            reply->header.cmd_num = common_msg_ack;
            memcpyx(aro_key, cmd_data->aes_key, sizeof(aro_key));
            break;

        case radio_msg_get_main_key:
            // 0 for gs1 key, 1 for gs2 key, 2 for signing key
            // TODO: revisit
            if (cmd->data[0] < 3) {
                reply->header.cmd_num = radio_msg_main_key;
                memcpyx(reply_data->aes_key, (__xdata uint8_t *) (flash_keys + (cmd->data[0] * AES_KEY_SIZE)), sizeof(reply_data->aes_key));
                reply_length += sizeof(reply_data->aes_key);
            }
            else {
                reply->header.cmd_num = common_msg_nack;
            }
            break;

        case radio_update_telem:
            reply->header.cmd_num = common_msg_ack;
            update_telemetry();
            break;

        case radio_send_test_data:
            if (cmd->data[0] <= LOCAL_PACKET_DATA_MAX_NUM_BYTES) {
                reply->header.cmd_num = radio_send_test_data;
                reply_data->data[0] = cmd->data[0];
                reply_length += cmd->data[0];
            }
            else {
                reply->header.cmd_num = common_msg_nack;
            }
            break;


        #if RADIO_RANGING_RESPONDER == 1
        case radio_msg_ranging:
            reply->header.cmd_num = radio_msg_ranging_ack;
            // TODO handle encryption
            reply_data->ranging_ack.ack_type = RANGING_ACK_TYPE;
            reply_data->ranging_ack.ack_version= RANGING_ACK_VERSION;
            reply_length += sizeof(reply_data->ranging_ack);
            // TODO wait until timer edge

            // Send this packet using the ranging radio mode
            old_tx_mode = radio_mode_tx;
            radio_mode_tx = RADIO_MODE_RANGING_TX;
            radio_send_packet(reply, reply_length, RF_TIMING_PRECISE);
            // Restore the radio settings and mute the normal response
            radio_mode_tx = old_tx_mode;
            reply_length = 0;
            break;
        #endif

        #ifdef CUSTOM_COMMANDS
        default:
            reply_length = custom_commands(cmd, len, reply);
        #endif
    }
    return reply_length;
}
