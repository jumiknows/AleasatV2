/**
 * @file test_gps.c
 * @brief Unit tests for the GPS driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "gps_internals.h"
#include "gps_defs.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
}

void tearDown(void) {
}

void test_calculate_ctrl_msg_checksum(void) {
    uint8_t simple_checksum[2] = {0x01, 0x02};
    TEST_ASSERT_EQUAL_UINT8(calculate_ctrl_msg_checksum((uint8_t*)&simple_checksum, 0), (uint8_t)0x00);
    TEST_ASSERT_EQUAL_UINT8(calculate_ctrl_msg_checksum((uint8_t*)&simple_checksum, 1), (uint8_t)0x01);
    TEST_ASSERT_EQUAL_UINT8(calculate_ctrl_msg_checksum((uint8_t*)&simple_checksum, 2), (uint8_t)0x03);

    uint8_t larger_checksum[250];
    for (int i = 0; i < 256; i++) {
        larger_checksum[i] = i;
    }
    uint8_t expected = 0;
    for (int i = 0; i < 250; i++) {
        expected ^= larger_checksum[i];
    }
    TEST_ASSERT_EQUAL_UINT8(calculate_ctrl_msg_checksum((uint8_t*)&larger_checksum, 250), expected);
}

void test_is_pkts_checksum_valid(void) {
    uint8_t valid_gps_msg[GPS_ACK_PKT_SIZE] = {GPS_MSG_START_SEQ_0, GPS_MSG_START_SEQ_1, 0x00, 0x02, 0x02, 0x01, 0x03, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    TEST_ASSERT_TRUE(is_pkts_checksum_valid((uint8_t*)&valid_gps_msg, GPS_ACK_PKT_SIZE));

    uint8_t invalid_gps_msg[GPS_ACK_PKT_SIZE] = {GPS_MSG_START_SEQ_0, GPS_MSG_START_SEQ_1, 0x00, 0x02, GPS_SYS_MSG_ACK, 0x01, 0xa1, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    TEST_ASSERT_FALSE(is_pkts_checksum_valid((uint8_t*)&invalid_gps_msg, GPS_ACK_PKT_SIZE));
}

void test_parse_control_packet(void) {
    uint8_t valid_gps_msg[10] = {GPS_MSG_START_SEQ_0, GPS_MSG_START_SEQ_1, 0x00, 0x03, 0x01, 0x02, 0x03, 0x02, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    gps_packet_t gps_pkt;
    uint8_t buf[10] = {0};
    gps_pkt.payload = buf;
    TEST_ASSERT_EQUAL(GPS_PARSE_OK, parse_control_packet((uint8_t*)&valid_gps_msg, 10, &gps_pkt));
    TEST_ASSERT_TRUE(gps_pkt.ack);
    TEST_ASSERT_EQUAL_UINT8(gps_pkt.id, (uint8_t)1);
    TEST_ASSERT_EQUAL_UINT16(gps_pkt.len, (uint16_t)2);

    // Message too short.
    uint8_t short_gps_msg[8] = {0x00, 0x03, 0x01, 0x02, 0x03, 0x02, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    TEST_ASSERT_EQUAL(GPS_PARSE_TOO_SHORT, parse_control_packet((uint8_t*)&short_gps_msg, 8, &gps_pkt));

    // Only Ack.
    uint8_t ack_gps_msg[9] = {GPS_MSG_START_SEQ_0, GPS_MSG_START_SEQ_1, 0x00, 0x02, GPS_SYS_MSG_ACK, 0x02, 0x81, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    TEST_ASSERT_EQUAL(GPS_PARSE_OK, parse_control_packet((uint8_t*)&ack_gps_msg, 9, &gps_pkt));
    TEST_ASSERT_TRUE(gps_pkt.ack);
    TEST_ASSERT_EQUAL_UINT8(gps_pkt.id, (uint8_t)GPS_SYS_MSG_ACK);
    TEST_ASSERT_EQUAL_UINT16(gps_pkt.len, (uint16_t)0);

    // Only Nack.
    uint8_t nack_gps_msg[9] = {GPS_MSG_START_SEQ_0, GPS_MSG_START_SEQ_1, 0x00, 0x02, GPS_SYS_MSG_NACK, 0x02, GPS_MSG_END_SEQ_0, GPS_MSG_END_SEQ_1};
    TEST_ASSERT_EQUAL(GPS_PARSE_OK, parse_control_packet((uint8_t*)&nack_gps_msg, 9, &gps_pkt));
    TEST_ASSERT_FALSE(gps_pkt.ack);
    TEST_ASSERT_EQUAL_UINT8(gps_pkt.id, (uint8_t)GPS_SYS_MSG_NACK);
    TEST_ASSERT_EQUAL_UINT16(gps_pkt.len, (uint16_t)0);
}
