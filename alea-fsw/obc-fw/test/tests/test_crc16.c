/**
 * @file test_crc.c
 * @brief Unit tests for crc.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

// Utils
#include "obc_utils.h"
#include "obc_crc.h"

// Standard Library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
}

void tearDown(void) {
}

// Generate CRC16 Tests
/**
 * @brief Tests lower bound (empty buffer, data_len = 0)
 */
void test_crc_lower_bound(void) {
    char string[] = "";
    size_t length = strlen(string);
    uint16_t ref_crc = 0xffff;

    uint16_t crc16 = crc_16_buf(0xffff, string, length);

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests case of zero buffer
 */
void test_crc_zero_buf(void) {
    uint8_t input = 0x0;
    size_t length = sizeof(input);
    uint16_t ref_crc = 0x40bf;

    uint16_t crc16 = crc_16_buf(0xffff, &input, length);

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests upper bound (max buffer, data_len = 237)
 */
void test_crc_upper_bound(void) {
    char string[] = "WQFURYHZJSQMYZDTHGWMAFFYJHJDBNASCZDMGYNSJBNAMPXLHRRTCVBCXDDGALPRTHULXCGBQRYURBZJQEEXTEVFPPCJVMCEYFCDUDTMVGCFETYGHLGCYNVFGAGRKFCGQMQUXRZCFRCWZLDKMBPCZFAEMRMRJQCYKPMVMGZZLLYWHKKDWZERGEWYANVYQCQHWHFXHUTHPCHPLNTAVTEEULJJAQNPHXMTVGLRXZBZCXZUD";
    size_t length = strlen(string);
    uint16_t ref_crc = 0x7bd2;

    uint16_t crc16 = crc_16_buf(0xffff, string, length);

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests with random sample of data
 */
void test_crc_random1(void) {
    char string[] = "S32500000000EA00028EE59FF014E59FF014E59FF014E59FF014E59FF014E51FF1B0E51FF1B06E";
    size_t length = strlen(string);
    uint16_t ref_crc = 0x44ef;

    uint16_t crc16 = crc_16_buf(0xffff, string, length); 

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests with random sample of data
 */
void test_crc_random2(void) {
    char string[] = "S319000000200801FFC00801FFC40801FFC80801FFCC0801FFD0B6";
    size_t length = strlen(string);
    uint16_t ref_crc = 0xd802;

    uint16_t crc16 = crc_16_buf(0xffff, string, length); 

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests with random sample of data
 */
void test_crc_random3(void) {
    char string[] = "S32100000040FFFFFFFFA1EA50BC00002CF0112A84A710ADC0DE00000040EA0002788A";
    size_t length = strlen(string);
    uint16_t ref_crc = 0x1bd8;

    uint16_t crc16 = crc_16_buf(0xffff, string, length); 

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests with random sample of data
 */
void test_crc_random4(void) {
    char string[] = "S32500000060C001FACE00000000F2BE616E02010001000000000000000000000000000000006E";
    size_t length = strlen(string);
    uint16_t ref_crc = 0x414a;

    uint16_t crc16 = crc_16_buf(0xffff, string, length); 

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}

/**
 * @brief Tests with random sample of data
 */
void test_crc_random5(void) {
    char string[] = "S32500000080B51049B948B9600849B948BA600849BAF04F3001600849B9F2401001600849B835";
    size_t length = strlen(string);
    uint16_t ref_crc = 0x3861;

    uint16_t crc16 = crc_16_buf(0xffff, string, length); 

    TEST_ASSERT_EQUAL(ref_crc, crc16);
}
