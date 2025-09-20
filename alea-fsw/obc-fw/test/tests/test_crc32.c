/**
 * @file test_crc.c
 * @brief Unit tests for crc.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

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

// Generate CRC32 Tests
void test_crc_buf1(void) {
    char string[] = "S32500000000EA00028EE59FF014E59FF014E59FF014E59FF014E59FF014E51FF1B0E51FF1B06E";
    size_t length = strlen(string);
    uint32_t py_zlib_crc = 3213431362;

    uint32_t crc32 = crc_32_buf(CRC32_SEED, string, length);

    TEST_ASSERT_EQUAL(py_zlib_crc, crc32);
}

void test_crc_buf2(void) {
    char string[] = "S319000000200801FFC00801FFC40801FFC80801FFCC0801FFD0B6";
    size_t length = strlen(string);
    uint32_t py_zlib_crc = 866972984;

    uint32_t crc32 = crc_32_buf(CRC32_SEED, string, length);

    TEST_ASSERT_EQUAL(py_zlib_crc, crc32);
}

void test_crc_buf3(void) {
    char string[] = "S32100000040FFFFFFFFA1EA50BC00002CF0112A84A710ADC0DE00000040EA0002788A";
    size_t length = strlen(string);
    uint32_t py_zlib_crc = 1583740130;

    uint32_t crc32 = crc_32_buf(CRC32_SEED, string, length);

    TEST_ASSERT_EQUAL(py_zlib_crc, crc32);
}

void test_crc_buf4(void) {
    char string[] = "S32500000060C001FACE00000000F2BE616E02010001000000000000000000000000000000006E";
    size_t length = strlen(string);
    uint32_t py_zlib_crc = 721719557;

    uint32_t crc32 = crc_32_buf(CRC32_SEED, string, length);

    TEST_ASSERT_EQUAL(py_zlib_crc, crc32);
}

void test_crc_buf5(void) {
    char string[] = "S32500000080B51049B948B9600849B948BA600849BAF04F3001600849B9F2401001600849B835";
    size_t length = strlen(string);
    uint32_t py_zlib_crc = 3443221350;

    uint32_t crc32 = crc_32_buf(CRC32_SEED, string, length);

    TEST_ASSERT_EQUAL(py_zlib_crc, crc32);
}

/******************************************************************************/
/*                         P Y T H O N  P R O G R A M                         */
/******************************************************************************/
// import zlib

// data = b"S32500000000EA00028EE59FF014E59FF014E59FF014E59FF014E59FF014E51FF1B0E51FF1B06E"
// crc = zlib.crc32(data)
// print(crc) # 3213431362

// data = b"S319000000200801FFC00801FFC40801FFC80801FFCC0801FFD0B6"
// crc = zlib.crc32(data)
// print(crc) # 866972984

// data = b"S32100000040FFFFFFFFA1EA50BC00002CF0112A84A710ADC0DE00000040EA0002788A"
// crc = zlib.crc32(data)
// print(crc) # 1583740130

// data = b"S32500000060C001FACE00000000F2BE616E02010001000000000000000000000000000000006E"
// crc = zlib.crc32(data)
// print(crc) # 721719557

// data = b"S32500000080B51049B948B9600849B948BA600849BAF04F3001600849B9F2401001600849B835"
// crc = zlib.crc32(data)
// print(crc) # 3443221350
