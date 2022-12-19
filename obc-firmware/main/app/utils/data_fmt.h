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

// OBC
#include "obc_misra.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    DATA_FMT_FIELD_TYPE_U32,
    DATA_FMT_FIELD_TYPE_U16,
    DATA_FMT_FIELD_TYPE_U8,
    DATA_FMT_FIELD_TYPE_BOOL,

    DATA_FMT_FIELD_TYPE_COUNT,
} data_fmt_field_type_t;

typedef struct {
    data_fmt_field_type_t type;
    uint8_t struct_offset;
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

OBC_MISRA_CHECK_OFF

/* ----------------------------- SERIALIZATION ------------------------------ */

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

/* ---------------------------- DE-SERIALIZATION ---------------------------- */

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

OBC_MISRA_CHECK_ON

#endif // DATA_FMT_H_
