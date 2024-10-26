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

#define TEST_STRUCT_SERIALIZED_SIZE 84

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    uint8_t  u8_1;
    uint16_t u16_1;
    uint32_t u32_1;
    uint64_t u64_1;
    float32  f32_1;
    float64  f64_1;
    bool     b_1;
    uint8_t  u8_2[2];
    uint16_t u16_2[2];
    uint32_t u32_2[2];
    uint64_t u64_2[2];
    float32  f32_2[2];
    float64  f64_2[2];
    bool     b_2[2];
} test_struct_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static test_struct_t test_struct = {
    .u8_1  = 0x12,
    .u16_1 = 0x3456,
    .u32_1 = 0x789ABCDE,
    .u64_1 = 0x13572468ACE1BDF2,
    .f32_1 = 2.718281828,
    .f64_1 = 1.61803398875,
    .b_1   = true,
    .u8_2  = { 0xA1, 0xEA},
    .u16_2 = { 0x5A74, 0xDEAD},
    .u32_2 = { 0xBEEFF00D, 0x29624628},
    .u64_2 = { 0x2A966C24F297B5D2, 0xB1CAE88215D1F353},
    .f32_2 = { -1.41421356237, 2.2360679775},
    .f64_2 = { 1054571.817646156391262, -0.0072973525693},
    .b_2   = { false, true},
};

static uint8_t expected_serialized[TEST_STRUCT_SERIALIZED_SIZE] = {
    0x12,
    0x34, 0x56,
    0x78, 0x9A, 0xBC, 0xDE,
    0x13, 0x57, 0x24, 0x68, 0xAC, 0xE1, 0xBD, 0xF2,
    0x40, 0x2D, 0xF8, 0x54,
    0x3F, 0xF9, 0xE3, 0x77, 0x9B, 0x97, 0xF6, 0x81,
    0x01,
    // ^ 28 bytes

    0xA1, 0xEA,
    0x5A, 0x74, 0xDE, 0xAD,
    0xBE, 0xEF, 0xF0, 0x0D, 0x29, 0x62, 0x46, 0x28,
    0x2A, 0x96, 0x6C, 0x24, 0xF2, 0x97, 0xB5, 0xD2, 0xB1, 0xCA, 0xE8, 0x82, 0x15, 0xD1, 0xF3, 0x53,
    0xBF, 0xB5, 0x04, 0xF3, 0x40, 0x0F, 0x1B, 0xBD,
    0x41, 0x30, 0x17, 0x6B, 0xD1, 0x51, 0x42, 0x2D, 0xBF, 0x7D, 0xE3, 0xD4, 0x2A, 0x1E, 0xD2, 0x9D,
    0x00, 0x01,
    // ^ 56 bytes
};

static data_fmt_field_desc_t field_descs[14] = {
    { .type = DATA_FMT_FIELD_TYPE_U8,   .struct_offset = offsetof(test_struct_t, u8_1),  .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_U16,  .struct_offset = offsetof(test_struct_t, u16_1), .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_U32,  .struct_offset = offsetof(test_struct_t, u32_1), .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_U64,  .struct_offset = offsetof(test_struct_t, u64_1), .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_F32,  .struct_offset = offsetof(test_struct_t, f32_1), .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_F64,  .struct_offset = offsetof(test_struct_t, f64_1), .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_BOOL, .struct_offset = offsetof(test_struct_t, b_1),   .array_len = 1 },
    { .type = DATA_FMT_FIELD_TYPE_U8,   .struct_offset = offsetof(test_struct_t, u8_2),  .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_U16,  .struct_offset = offsetof(test_struct_t, u16_2), .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_U32,  .struct_offset = offsetof(test_struct_t, u32_2), .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_U64,  .struct_offset = offsetof(test_struct_t, u64_2), .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_F32,  .struct_offset = offsetof(test_struct_t, f32_2), .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_F64,  .struct_offset = offsetof(test_struct_t, f64_2), .array_len = 2 },
    { .type = DATA_FMT_FIELD_TYPE_BOOL, .struct_offset = offsetof(test_struct_t, b_2),   .array_len = 2 },
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

void test_serialize_field_U64(void) {
    uint8_t buf[8] = { 0 };

    const uint64_t data = 0x123456789ABCDEFE;
    const uint8_t expected[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFE };
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U64, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_U64_bufTooSmall(void) {
    uint8_t buf[7] = { 0 };

    const uint64_t data = 0x123456789ABCDEFE;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_U64, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_F32(void) {
    uint8_t buf[4] = { 0 };

    const float32 data = -3.141592;
    const uint8_t expected[4] = { 0xC0, 0x49, 0x0F, 0xD8 }; // pre-calculated
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_F32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_F32_bufTooSmall(void) {
    uint8_t buf[3] = { 0 };

    const float32 data = -3.141592;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_F32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(0, len);
}

void test_serialize_field_F64(void) {
    uint8_t buf[8] = { 0 };

    const float64 data = -3.141592;
    const uint8_t expected[8] = { 0xC0, 0x09, 0x21, 0xFA, 0xFC, 0x8B, 0x00, 0x7A }; // pre-calculated
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_F64, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(expected), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
}

void test_serialize_field_F64_bufTooSmall(void) {
    uint8_t buf[7] = { 0 };

    const float64 data = -3.141592;
    uint32_t len = data_fmt_serialize_field(DATA_FMT_FIELD_TYPE_F64, &data, buf, sizeof(buf));

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

void test_deserialize_field_U64(void) {
    uint64_t data = 0;

    const uint8_t buf[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFE };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U64, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL(0x123456789ABCDEFE, data);
}

void test_deserialize_field_U64_bufTooSmall(void) {
    uint32_t data = 0;

    const uint8_t buf[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFE };
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_U64, &data, buf, 7);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_F32(void) {
    float32 data = 0;

    const uint8_t buf[4] = { 0xC0, 0x49, 0x0F, 0xD8 }; // pre-calculated
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_F32, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL((float32)-3.141592, data);
}

void test_deserialize_field_F32_bufTooSmall(void) {
    float32 data = 0;

    const uint8_t buf[4] = { 0xC0, 0x49, 0x0F, 0xD8 }; // pre-calculated
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_F32, &data, buf, 3);

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_field_F64(void) {
    float64 data = 0;

    const uint8_t buf[8] = { 0xC0, 0x09, 0x21, 0xFA, 0xFC, 0x8B, 0x00, 0x7A }; // pre-calculated
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_F64, &data, buf, sizeof(buf));

    TEST_ASSERT_EQUAL(sizeof(buf), len);
    TEST_ASSERT_EQUAL((float64)-3.141592, data);
}

void test_deserialize_field_F64_bufTooSmall(void) {
    float64 data = 0;

    const uint8_t buf[8] = { 0xC0, 0x09, 0x21, 0xFA, 0xFC, 0x8B, 0x00, 0x7A }; // pre-calculated
    uint32_t len = data_fmt_deserialize_field(DATA_FMT_FIELD_TYPE_F64, &data, buf, 7);

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

    uint32_t len = data_fmt_deserialize_struct(&data, &desc, expected_serialized, (TEST_STRUCT_SERIALIZED_SIZE - 1));

    TEST_ASSERT_EQUAL(0, len);
}

void test_deserialize_struct_ok(void) {
    test_struct_t data = { 0 };

    uint32_t len = data_fmt_deserialize_struct(&data, &desc, expected_serialized, TEST_STRUCT_SERIALIZED_SIZE);

    TEST_ASSERT_EQUAL(TEST_STRUCT_SERIALIZED_SIZE, len);

    TEST_ASSERT_EQUAL(test_struct.u8_1,  data.u8_1);
    TEST_ASSERT_EQUAL(test_struct.u16_1, data.u16_1);
    TEST_ASSERT_EQUAL(test_struct.u32_1, data.u32_1);
    TEST_ASSERT_EQUAL(test_struct.u64_1, data.u64_1);
    TEST_ASSERT_EQUAL(test_struct.f32_1, data.f32_1);
    TEST_ASSERT_EQUAL(test_struct.f64_1, data.f64_1);
    TEST_ASSERT_EQUAL(test_struct.b_1,   data.b_1);

    TEST_ASSERT_EQUAL(test_struct.u8_2[0],  data.u8_2[0]);
    TEST_ASSERT_EQUAL(test_struct.u8_2[1],  data.u8_2[1]);
    TEST_ASSERT_EQUAL(test_struct.u16_2[0], data.u16_2[0]);
    TEST_ASSERT_EQUAL(test_struct.u16_2[1], data.u16_2[1]);
    TEST_ASSERT_EQUAL(test_struct.u32_2[0], data.u32_2[0]);
    TEST_ASSERT_EQUAL(test_struct.u32_2[1], data.u32_2[1]);
    TEST_ASSERT_EQUAL(test_struct.u64_2[0], data.u64_2[0]);
    TEST_ASSERT_EQUAL(test_struct.u64_2[1], data.u64_2[1]);
    TEST_ASSERT_EQUAL(test_struct.f32_2[0], data.f32_2[0]);
    TEST_ASSERT_EQUAL(test_struct.f32_2[1], data.f32_2[1]);
    TEST_ASSERT_EQUAL(test_struct.f64_2[0], data.f64_2[0]);
    TEST_ASSERT_EQUAL(test_struct.f64_2[1], data.f64_2[1]);
    TEST_ASSERT_EQUAL(test_struct.b_2[0],   data.b_2[0]);
    TEST_ASSERT_EQUAL(test_struct.b_2[1],   data.b_2[1]);
}
