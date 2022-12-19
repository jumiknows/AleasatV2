/**
 * @file test_sequence.c
 * @brief Unit tests for sequence.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "data_fmt.h"

// Utils
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TEST_STRUCT_SERIALIZED_SIZE 16

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    uint32_t u32_1;
    uint16_t u16_1;
    uint8_t u8_1;
    bool b_1;
    uint32_t u32_2;
    uint16_t u16_2;
    uint8_t u8_2;
    bool b_2;
} test_struct_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static test_struct_t test_struct = {
    .u32_1 = 0x12345678,
    .u16_1 = 0xABCD,
    .u8_1  = 0xEF,
    .b_1   = false,
    .u32_2 = 0xFEDCBA98,
    .u16_2 = 0x7654,
    .u8_2  = 0x32,
    .b_2   = true,
};

static uint8_t expected_serialized[TEST_STRUCT_SERIALIZED_SIZE] = {
    0x12, 0x34, 0x56, 0x78,
    0xAB, 0xCD,
    0xEF,
    0x00,

    0xFE, 0xDC, 0xBA, 0x98,
    0x76, 0x54,
    0x32,
    0x01
};

static data_fmt_field_desc_t field_descs[8] = {
    { .type = DATA_FMT_FIELD_TYPE_U32,  .struct_offset = offsetof(test_struct_t, u32_1) },
    { .type = DATA_FMT_FIELD_TYPE_U16,  .struct_offset = offsetof(test_struct_t, u16_1) },
    { .type = DATA_FMT_FIELD_TYPE_U8,   .struct_offset = offsetof(test_struct_t, u8_1)  },
    { .type = DATA_FMT_FIELD_TYPE_BOOL, .struct_offset = offsetof(test_struct_t, b_1)   },
    { .type = DATA_FMT_FIELD_TYPE_U32,  .struct_offset = offsetof(test_struct_t, u32_2) },
    { .type = DATA_FMT_FIELD_TYPE_U16,  .struct_offset = offsetof(test_struct_t, u16_2) },
    { .type = DATA_FMT_FIELD_TYPE_U8,   .struct_offset = offsetof(test_struct_t, u8_2)  },
    { .type = DATA_FMT_FIELD_TYPE_BOOL, .struct_offset = offsetof(test_struct_t, b_2)   },
};

static data_fmt_desc_t desc = {
    .fields = field_descs,
    .count = LEN(field_descs),
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
}

void tearDown(void) {
}

// Serialize Field Tests

void test_serialize_field_illegalType(void) {
    uint8_t buf[4] = { 0 };
    const uint32_t data = 0x12345678;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_COUNT, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_nullData(void) {
    uint8_t buf[4] = { 0 };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U32, NULL, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_nullBuf(void) {
    const uint32_t data = 0x12345678;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U32, &data, NULL, 4);

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_U32(void) {
    uint8_t buf[4] = { 0 };

    const uint32_t data = 0x12345678;
    const uint8_t expected[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_U32_bufTooSmall(void) {
    uint8_t buf[3] = { 0 };

    const uint32_t data = 0x12345678;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_U16(void) {
    uint8_t buf[2] = { 0 };

    const uint16_t data = 0x1234;
    const uint8_t expected[2] = { 0x12, 0x34 };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U16, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_U16_bufTooSmall(void) {
    uint8_t buf[1] = { 0 };

    const uint16_t data = 0x1234;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U16, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_U8(void) {
    uint8_t buf[1] = { 0 };

    const uint8_t data = 0x12;
    const uint8_t expected[1] = { 0x12 };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U8, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_U8_bufTooSmall(void) {
    uint8_t buf[1] = { 0 };

    const uint8_t data = 0x12;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U8, &data, buf, 0);

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_bool(void) {
    uint8_t buf[1] = { 0 };

    const bool data = true;
    const uint8_t expected[1] = { 0x01 };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_BOOL, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_bool_bufTooSmall(void) {
    uint8_t buf[1] = { 0 };

    bool data = true;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_BOOL, &data, buf, 0);

    TEST_ASSERT_EQUAL(0, len);
}

// Serialize Struct Tests

void test_serialize_struct_nullData(void) {
    uint8_t buf[TEST_STRUCT_SERIALIZED_SIZE] = { 0 };

    uint32_t len = data_fmt_serialize_struct(NULL, &desc, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_struct_nullDesc(void) {
    uint8_t buf[TEST_STRUCT_SERIALIZED_SIZE] = { 0 };

    uint32_t len = data_fmt_serialize_struct(&test_struct, NULL, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_struct_nullBuf(void) {
    uint8_t buf[TEST_STRUCT_SERIALIZED_SIZE] = { 0 };

    uint32_t len = data_fmt_serialize_struct(&test_struct, &desc, NULL, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_struct_bufTooSmall(void) {
    uint8_t buf[TEST_STRUCT_SERIALIZED_SIZE - 1] = { 0 };

    uint32_t len = data_fmt_serialize_struct(&test_struct, &desc, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_struct_ok(void) {
    uint8_t buf[TEST_STRUCT_SERIALIZED_SIZE] = { 0 };

    uint32_t len = data_fmt_serialize_struct(&test_struct, &desc, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(TEST_STRUCT_SERIALIZED_SIZE, len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_serialized, buf, TEST_STRUCT_SERIALIZED_SIZE);
}

// Deserialize Field Tests

void test_deserialize_field_illegalType(void) {
    uint32_t data = 0;
    const uint8_t buf[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_COUNT, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_nullData(void) {
    const uint8_t buf[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U32, NULL, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_nullBuf(void) {
    uint32_t data = 0;
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U32, &data, NULL, 4);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_U32(void) {
    uint32_t data = 0;

    const uint8_t buf[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL(0x12345678, data);
}

void test_deserialize_field_U32_bufTooSmall(void) {
    uint32_t data = 0;

    const uint8_t buf[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U32, &data, buf, 3);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_U16(void) {
    uint16_t data = 0;

    const uint8_t buf[2] = { 0x12, 0x34 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U16, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL(0x1234, data);
}

void test_deserialize_field_U16_bufTooSmall(void) {
    uint16_t data = 0;

    const uint8_t buf[2] = { 0x12, 0x34 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U16, &data, buf, 1);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_U8(void) {
    uint8_t data = 0;

    const uint8_t buf[1] = { 0x12 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U8, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL(0x12, data);
}

void test_deserialize_field_U8_bufTooSmall(void) {
    uint8_t data = 0;

    const uint8_t buf[1] = { 0x12 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U8, &data, buf, 0);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_bool(void) {
    bool data = 0;

    const uint8_t buf[1] = { 0x01 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_BOOL, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_TRUE(data);
}

void test_deserialize_field_bool_bufTooSmall(void) {
    bool data = 0;

    const uint8_t buf[1] = { 0x01 };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_BOOL, &data, buf, 0);

    TEST_ASSERT_EQUAL(0, len);
}

// Deserialize Struct Tests

void test_deserialize_struct_nullData(void) {
    uint32_t len = data_fmt_deserialize_struct(NULL, &desc, expected_serialized, sizeof(expected_serialized));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_struct_nullDesc(void) {
    test_struct_t data = { 0 };

    uint32_t len = data_fmt_deserialize_struct(&data, NULL, expected_serialized, sizeof(expected_serialized));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_struct_nullBuf(void) {
    test_struct_t data = { 0 };

    uint32_t len = data_fmt_deserialize_struct(&data, &desc, NULL, sizeof(expected_serialized));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_struct_bufTooSmall(void) {
    test_struct_t data = { 0 };

    uint32_t len = data_fmt_deserialize_struct(&data, &desc, expected_serialized, (sizeof(expected_serialized) - 1));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_struct_ok(void) {
    test_struct_t data = { 0 };

    uint32_t len = data_fmt_deserialize_struct(&data, &desc, expected_serialized, sizeof(expected_serialized));

    TEST_ASSERT_EQUAL(TEST_STRUCT_SERIALIZED_SIZE, len);

    TEST_ASSERT_EQUAL(test_struct.u32_1, data.u32_1);
    TEST_ASSERT_EQUAL(test_struct.u16_1, data.u16_1);
    TEST_ASSERT_EQUAL(test_struct.u8_1 , data.u8_1);
    TEST_ASSERT_EQUAL(test_struct.b_1  , data.b_1);

    TEST_ASSERT_EQUAL(test_struct.u32_2, data.u32_2);
    TEST_ASSERT_EQUAL(test_struct.u16_2, data.u16_2);
    TEST_ASSERT_EQUAL(test_struct.u8_2 , data.u8_2);
    TEST_ASSERT_EQUAL(test_struct.b_2  , data.b_2);
}
