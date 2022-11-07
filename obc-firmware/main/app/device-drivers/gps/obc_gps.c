/*
 * @file obc_gps.c
 * @brief Venus838LPx-T-L-1738D01 GPS module driver
 * //TODO confirm if we have the L or D version for flatsat.
 *
 * Message Format of a GPS message.
 *
 * Start of Sequence:  2 bytes (0xA0, 0xA1).
 * Payload Length:     2 bytes.
 * Message ID:         1 Byte.
 * Message Body: up to 65535 Bytes.
 * Checksum:           1 Byte.
 * End of sequence:    2 Bytes. (0x0D, 0x0A).
 *
 * A Message Body may further consist of 2 sub-fields, Sub-Message ID (Sub ID)  and Sub-Message Body.
 * Sub-Message ID:     1 Byte.
 * Sub-Message Body:   65534 Bytes.
 *
 * The checksum is the 8-bit exclusive OR of only the payload bytes which start from Message IS until the checksum byte.
 *
 * The message is transferred in big-endian format. The high order byte is transmitted first.
 */

/**
 * Message Flow
 * The GPS can recieve messages from a host. In this case the OBC.
 * Upon receiving a successful input message, the GPS will first ACK the request and then reply to the message.
 * 1. GPS <- MSG -- OBC
 * 2. GPS -- ACK -> OBC
 * 3. GPS -- RPL -> OBC
 *
 * Upon receiving an uncessful input message, the GPS will NACK the request.
 * 1. GPS <- MSG --- OBC
 * 2. GPS -- NACK -> OBC
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_gps.h"

// OBC
#include "logger.h"

// std lib
#include <stdint.h>

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

uint8_t calculate_msg_checksum(uint8_t* msg, uint8_t size);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Sends the GPS message which will reset and restart the GNSS receiver.
 *
 * There are three types of start types:
 * HOT_START: Its when the GPS device remembers its last calculated postion
 *            and the satellites in view, the almanac used, the UTC time and
 * 			  tries to lock into the same satellites to lock a position.
 *
 * WARM_START: Its when the GPS device remembers its last calculated position,
 * 			   almanac used, and UTC time, but not which satellites were in view.
 *
 * COLD_START: Its when the GPS device dumps all the information, and attempts
 * 			   to locate satellites and the ncalculates a GPS lock.
 */
gps_err_t gps_restart_receiver(gps_restart_modes_t restart_mode) {
    // We need to get the time time on the satellite.
    real_time_t curr_time = {0};
    rtc_err_t err         = rtc_get_current_time(&curr_time);
    if (err != RTC_NO_ERR) {
        return GPS_RTC_ERROR;
    }
    uint8_t year0  = ((curr_time.year + START_CENTURY) >> (8 * 0)) & 0xff;
    uint8_t year1  = ((curr_time.year + START_CENTURY) >> (8 * 1)) & 0xff;
    uint8_t month  = curr_time.month & 0xff;
    uint8_t day    = curr_time.month & 0xff;
    uint8_t hour   = curr_time.hour & 0xff;
    uint8_t minute = curr_time.minute & 0xff;
    uint8_t second = curr_time.second & 0xff;
    int8_t lat0    = (GPS_YUL_LAT >> (8 * 0)) & 0xff;
    int8_t lat1    = (GPS_YUL_LAT >> (8 * 1)) & 0xff;
    int8_t lon0    = (GPS_YUL_LON >> (8 * 0)) & 0xff;
    int8_t lon1    = (GPS_YUL_LON >> (8 * 1)) & 0xff;
    int8_t alt0    = (GPS_YUL_ALT >> (8 * 0)) & 0xff;
    int8_t alt1    = (GPS_YUL_ALT >> (8 * 1)) & 0xff;

    uint8_t payload[15] = {GPS_SYS_CMD_RESART, restart_mode, year0, year1, month, day, hour, minute, second, lat0, lat1, lon0, lon1, alt0, alt1};

    uint8_t checksum = calculate_msg_checksum(payload, 15);

    // TODO CREATE FUNC TO ADJUST ORCATIME TO NORMAL TIME.
    // TODO make a func that checks the checksum and then appends the headers.
    // Maybe returns the entire msg.
    uint8_t gps_restart_msg[GPS_RESTART_MSG_LEN] = {(uint8_t)GPS_MSG_START_SEQ_0,
                                                    (uint8_t)GPS_MSG_START_SEQ_1,
                                                    0x00,
                                                    0x0F,
                                                    (uint8_t)GPS_SYS_CMD_RESART,
                                                    (uint8_t)restart_mode,
                                                    year1,
                                                    year0,
                                                    month,
                                                    day,
                                                    hour,
                                                    minute,
                                                    second,
                                                    lat1,
                                                    lat0,
                                                    lon1,
                                                    lon0,
                                                    alt1,
                                                    alt0,
                                                    checksum,
                                                    (uint8_t)GPS_MSG_END_SEQ_0,
                                                    (uint8_t)GPS_MSG_END_SEQ_1};
    gps_send((uint8_t*)gps_restart_msg, GPS_RESTART_MSG_LEN);
    return GPS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Calculates the checksum of a GPS message
 *
 * Implementation note on Page 4.
 *
 * @param msg GPS message including MSG ID.
 * @param size length of GPS message.
 * @return uint8_t XOR checksum bit.
 */
uint8_t calculate_msg_checksum(uint8_t* msg, uint8_t size) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < size; i++) {
        checksum = checksum ^ msg[i];
    }
    return checksum;
}
