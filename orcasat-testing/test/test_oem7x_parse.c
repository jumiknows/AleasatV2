/**
 * @file test_oem7x_parse.c
 * @brief OEM7x GPS module - message parsing unit tests
 * @author Andres Martinez
 * @date April 6, 2020
 *
 * @ref
 * https://docs.google.com/document/d/1oi1wM5Jj_66x2hNn_YKbEKmLwidT4hOdgek27ldUE2I/edit#heading=h.3auxtdt85ry5
 * "GPS module integration document"
 * @ref https://docs.novatel.com/oem7/Content/PDFs/OEM7_Installation_Operation_Manual.pdf
 * "Installation and Operation Manual"
 * @ref https://docs.novatel.com/oem7/Content/PDFs/OEM7_Commands_Logs_Manual.pdf "Command and Logs
 * Reference Manual"
 */

#include "unity.h"

#include "oem7x_parse.h"

#include <stdlib.h>
#include <string.h>

// floating point comparison
#define FP_DELTA 0.0001

/**
 * @brief Tests for OEM7X message parsing.
 */

// Tests
void test_bestxyz_valid();
void test_bestxyz_nosync();
void test_bestxyz_truncated();
void test_bestxyz_wrong_type();
void test_timesync_valid();
void test_timesync_nosync();
void test_timesync_truncated();
void test_timesync_wrong_type();

// Helper functions
static uint8_t real_close(double a, double b);

/**
 * @brief Test cases. Valid and invalid OEM7X ASCII messages
 */
static const char* BESTXYZ_VALID = "<OK\r\n[COM1]#BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;"
                                   "SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,"
                                   "0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,0.0199,0.0439,"
                                   "0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";

static const char* BESTXYZ_TRUNCATED = "<OK\r\n[COM1]#BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;"
                                       "SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,"
                                       "0.0219,0.0115,SOL_COMPUTED,NARROW_INT";

static const char* TIMESYNC_VALID = "<OK\r\n[COM1]#TIMESYNCA,COM1,0,46.0,FINESTEERING,1337,410095.000,02000000,bd3f,1984;"
                                    "1337,410095000,FINESTEERING*aa2025db";

static const char* TIMESYNC_TRUNCATED = "<OK\r\n[COM1]#TIMESYNCA,COM1,0,46.0,FINESTEERING,1337,410095.000,02000000,bd3f,1984;"
                                        "1337";

static char msg[300];

/**
 * @brief Floating point equality comparison
 *
 *  @param a: Comparison operand
 *  @param b: Comparison operand
 *
 *  @return 1 if numbers are approximately equal, 0 otherwise
 */
static uint8_t real_close(double a, double b) {
    return (fabs(a - b) < FP_DELTA);
}

/**
 * @brief Tests valid BESTXYZ message parsing
 */
void test_bestxyz_valid() {
    gps_ecef_t ec;
    strcpy(msg, BESTXYZ_VALID);
    const gps_parse_err_t err = parse_gps_bestxyz_ascii(msg, &ec);

    TEST_ASSERT_TRUE(err == GPS_PARSE_SUCCESS);

    TEST_ASSERT_EQUAL_STRING(ec.p_sol_status, "SOL_COMPUTED");
    TEST_ASSERT_EQUAL_STRING(ec.v_sol_status, "SOL_COMPUTED");

    TEST_ASSERT_TRUE(real_close(ec.px_m, -1634531.5683));
    TEST_ASSERT_TRUE(real_close(ec.py_m, -3664618.0326));
    TEST_ASSERT_TRUE(real_close(ec.pz_m, 4942496.3270));
    TEST_ASSERT_TRUE(real_close(ec.px_sd_m, 0.0099));
    TEST_ASSERT_TRUE(real_close(ec.py_sd_m, 0.0219));
    TEST_ASSERT_TRUE(real_close(ec.pz_sd_m, 0.0115));

    TEST_ASSERT_TRUE(real_close(ec.vx_mps, 0.0011));
    TEST_ASSERT_TRUE(real_close(ec.vy_mps, -0.0049));
    TEST_ASSERT_TRUE(real_close(ec.vz_mps, -0.0001));
    TEST_ASSERT_TRUE(real_close(ec.vx_sd_mps, 0.0199));
    TEST_ASSERT_TRUE(real_close(ec.vy_sd_mps, 0.0439));
    TEST_ASSERT_TRUE(real_close(ec.vz_sd_mps, 0.0230));

    TEST_ASSERT_TRUE(real_close(ec.sol_age_s, 0.0000));
}

/**
 * @brief Tests BESTXYZ message parsing with missing sync
 */
void test_bestxyz_nosync() {
    gps_ecef_t ec;
    strcpy(msg, BESTXYZ_VALID);
    msg[4]                    = 0;
    const gps_parse_err_t err = parse_gps_bestxyz_ascii(msg, &ec);

    TEST_ASSERT_TRUE(err == GPS_PARSE_NO_SYNC);
}

/**
 * @brief Tests truncated BESTXYZ message parsing
 */
void test_bestxyz_truncated() {
    gps_ecef_t ec;
    strcpy(msg, BESTXYZ_TRUNCATED);
    const gps_parse_err_t err = parse_gps_bestxyz_ascii(msg, &ec);

    TEST_ASSERT_TRUE(err == GPS_PARSE_MATCH_COUNT);
}

/**
 * @brief Tests BESTXYZ wrong message type
 */
void test_bestxyz_wrong_type() {
    gps_ecef_t ec;
    strcpy(msg, TIMESYNC_VALID);
    const gps_parse_err_t err = parse_gps_bestxyz_ascii(msg, &ec);

    TEST_ASSERT_TRUE(err == GPS_PARSE_MSG_TYPE);
}

/**
 * @brief Tests valid TIMESYNC message parsing
 */
void test_timesync_valid() {
    gps_time_t tr;
    strcpy(msg, TIMESYNC_VALID);
    const gps_parse_err_t err = parse_gps_timesync_ascii(msg, &tr);

    TEST_ASSERT_TRUE(err == GPS_PARSE_SUCCESS);

    TEST_ASSERT_EQUAL_INT(tr.ref_week, 1337);
    TEST_ASSERT_EQUAL_INT(tr.offset_ms, 410095000);
    TEST_ASSERT_EQUAL_STRING(tr.t_sol_status, "FINESTEERING");
}

/**
 * @brief Tests TIMESYNC message parsing with missing sync
 */
void test_timesync_nosync() {
    gps_time_t tr;
    strcpy(msg, TIMESYNC_VALID);
    msg[4]                    = 0;
    const gps_parse_err_t err = parse_gps_timesync_ascii(msg, &tr);

    TEST_ASSERT_TRUE(err == GPS_PARSE_NO_SYNC);
}

/**
 * @brief Tests truncated TIMESYNC message parsing
 */
void test_timesync_truncated() {
    gps_time_t tr;
    strcpy(msg, TIMESYNC_TRUNCATED);
    const gps_parse_err_t err = parse_gps_timesync_ascii(msg, &tr);

    TEST_ASSERT_TRUE(err == GPS_PARSE_MATCH_COUNT);
}

/**
 * @brief Tests TIMESYNC wrong message type
 */
void test_timesync_wrong_type() {
    gps_time_t tr;
    strcpy(msg, BESTXYZ_VALID);
    const gps_parse_err_t err = parse_gps_timesync_ascii(msg, &tr);

    TEST_ASSERT_TRUE(err == GPS_PARSE_MSG_TYPE);
}
