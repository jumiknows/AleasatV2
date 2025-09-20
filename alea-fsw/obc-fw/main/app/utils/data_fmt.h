/**
 * @file data_fmt.h
 * @brief Data serialization / deserialization to/from byte streams
 */

#ifndef DATA_FMT_H_
#define DATA_FMT_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    DATA_FMT_FIELD_TYPE_U8,
    DATA_FMT_FIELD_TYPE_U16,
    DATA_FMT_FIELD_TYPE_U32,
    DATA_FMT_FIELD_TYPE_U64,
    DATA_FMT_FIELD_TYPE_F32, // Warning: this type assumes the C float implementation uses IEEE-754 binary32
    DATA_FMT_FIELD_TYPE_F64, // Warning: this type assumes the C double implementation uses IEEE-754 binary64
    DATA_FMT_FIELD_TYPE_BOOL,

    DATA_FMT_FIELD_TYPE_COUNT,
} data_fmt_field_type_t;

typedef struct {
    data_fmt_field_type_t type;
    uint8_t struct_offset;
    uint8_t array_len;
} data_fmt_field_desc_t;

typedef struct {
    const data_fmt_field_desc_t *fields;
    uint8_t count;
} data_fmt_desc_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

uint32_t data_fmt_serialize_struct(const void *data, const data_fmt_desc_t *desc, uint8_t *buf, uint32_t buf_len);
uint32_t data_fmt_deserialize_struct(void *data, const data_fmt_desc_t *desc, const uint8_t *buf, uint32_t buf_len);

uint32_t data_fmt_serialize_field(data_fmt_field_type_t type, const void *data, uint8_t *buf, uint32_t buf_len);
uint32_t data_fmt_deserialize_field(data_fmt_field_type_t type, void *data, const uint8_t *buf, uint32_t buf_len);


/******************************************************************************/
/*                       I N L I N E  F U N C T I O N S                       */
/******************************************************************************/

/* ----------------------------- SERIALIZATION ------------------------------ */

/**
 * @brief Serialize a uint64_t to a byte array (big-endian)
 */
static inline void data_fmt_u64_to_arr_be(uint64_t value, uint8_t *arr) {
    arr[0] = GET_BYTE(value, 7);
    arr[1] = GET_BYTE(value, 6);
    arr[2] = GET_BYTE(value, 5);
    arr[3] = GET_BYTE(value, 4);
    arr[4] = GET_BYTE(value, 3);
    arr[5] = GET_BYTE(value, 2);
    arr[6] = GET_BYTE(value, 1);
    arr[7] = GET_BYTE(value, 0);
}

/**
 * @brief Serialize a uint32_t to a byte array (big-endian)
 */
static inline void data_fmt_u32_to_arr_be(uint32_t value, uint8_t *arr) {
    arr[0] = GET_BYTE(value, 3);
    arr[1] = GET_BYTE(value, 2);
    arr[2] = GET_BYTE(value, 1);
    arr[3] = GET_BYTE(value, 0);
}

/**
 * @brief Serialize a uint16_t to a byte array (big-endian)
 */
static inline void data_fmt_u16_to_arr_be(uint16_t value, uint8_t *arr) {
    arr[0] = GET_BYTE(value, 1);
    arr[1] = GET_BYTE(value, 0);
}

/**
 * @brief Serialize a 32-bit float to a byte array (big-endian) of IEEE-754 binary32 data
 *
 * @warning This assumes the C float implementation already uses IEEE-754 binary32
 */
static inline void data_fmt_f32_to_arr_be(float32 value, uint8_t *arr) {
    uint32_t value_int;
    memcpy(&value_int, &value, sizeof(value));
    data_fmt_u32_to_arr_be(value_int, arr);
}

/**
 * @brief Serialize a 64-bit double to a byte array (big-endian) of IEEE-754 binary64 data
 *
 * @warning This assumes the C double implementation already uses IEEE-754 binary64
 */
static inline void data_fmt_f64_to_arr_be(float64 value, uint8_t *arr) {
    uint64_t value_int;
    memcpy(&value_int, &value, sizeof(value));
    data_fmt_u64_to_arr_be(value_int, arr);
}

/* ---------------------------- DE-SERIALIZATION ---------------------------- */

/**
 * @brief Deserialize a uint64_t from a byte array (big-endian)
 */
static inline uint64_t data_fmt_arr_be_to_u64(const uint8_t *arr) {
    return (
               ((uint64_t)(arr[0])) << 56 |
               ((uint64_t)(arr[1])) << 48 |
               ((uint64_t)(arr[2])) << 40 |
               ((uint64_t)(arr[3])) << 32 |
               ((uint64_t)(arr[4])) << 24 |
               ((uint64_t)(arr[5])) << 16 |
               ((uint64_t)(arr[6])) <<  8 |
               ((uint64_t)(arr[7])) <<  0
           );
}

/**
 * @brief Deserialize a uint32_t from a byte array (big-endian)
 */
static inline uint32_t data_fmt_arr_be_to_u32(const uint8_t *arr) {
    return (
               ((uint32_t)(arr[0])) << 24 |
               ((uint32_t)(arr[1])) << 16 |
               ((uint32_t)(arr[2])) <<  8 |
               ((uint32_t)(arr[3])) <<  0
           );
}

/**
 * @brief Deserialize a uint16_t from a byte array (big-endian)
 */
static inline uint16_t data_fmt_arr_be_to_u16(const uint8_t *arr) {
    return (
               ((uint16_t)(arr[0])) << 8 |
               ((uint16_t)(arr[1])) << 0
           );
}

/**
 * @brief Deserialize a 32-bit float from a byte array (big-endian) of IEEE-754 binary32 data
 *
 * @warning This assumes the C float implementation already uses IEEE-754 binary32
 */
static inline float32 data_fmt_arr_be_to_f32(const uint8_t *arr) {
    float32 value = 0;
    uint32_t value_int = data_fmt_arr_be_to_u32(arr);
    memcpy(&value, &value_int, sizeof(value));
    return value;
}

/**
 * @brief Deserialize a 64-bit double from a byte array (big-endian) of IEEE-754 binary64 data
 *
 * @warning This assumes the C double implementation already uses IEEE-754 binary64
 */
static inline float64 data_fmt_arr_be_to_f64(const uint8_t *arr) {
    float64 value = 0;
    uint64_t value_int = data_fmt_arr_be_to_u64(arr);
    memcpy(&value, &value_int, sizeof(value));
    return value;
}

#endif // DATA_FMT_H_
