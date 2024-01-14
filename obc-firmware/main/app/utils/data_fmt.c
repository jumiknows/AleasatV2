/**
 * @file data_fmt.c
 * @brief Data serialization / deserialization to/from byte streams
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "data_fmt.h"

// Utils
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Serialize a data struct to a byte array according to the provided description
 *
 * @param[in]  data    Pointer to the data struct
 * @param[in]  desc    Pointer to the description of the data fields
 * @param[out] buf     Pointer to the buffer where the serialized data will be written
 * @param[in]  buf_len Length of buf
 *
 * @return Number of bytes produced by the serialization process or 0 if an error occurred
 */
uint32_t data_fmt_serialize_struct(const void *data, const data_fmt_desc_t *desc, uint8_t *buf, uint32_t buf_len) {
    // Check arguments
    if ((data == NULL) || (desc == NULL) || (buf == NULL)) {
        return 0;
    }

    uint32_t bytes_written = 0;

    // Iterate over the fields
    for (uint8_t i = 0; i < desc->count; i++) {
        const data_fmt_field_desc_t *field_desc = &(desc->fields[i]);
        // Create pointer to data field
        const uint8_t *field = (((const uint8_t *)data) + field_desc->struct_offset);

        for (uint8_t j = 0; j < field_desc->array_len; j++) {
            // Serialize the field
            uint32_t field_size = data_fmt_serialize_field(field_desc->type, field, buf, (buf_len - bytes_written));

            if (field_size == 0) {
                return 0;
            }

            // Update vars (the serialized and deserialized representation of the data are assumed to be the same size)
            field += field_size;
            buf += field_size;
            bytes_written += field_size;
        }
    }

    return bytes_written;
}

/**
 * @brief Deerialize a byte array to a data struct according to the provided description
 *
 * @param[out] data    Pointer to the data struct
 * @param[in]  desc    Pointer to the description of the data fields
 * @param[in]  buf     Pointer to the byte buffer
 * @param[in]  buf_len Length of buf
 *
 * @return Number of bytes consumed by the deserialization process or 0 if an error occurred
 */
uint32_t data_fmt_deserialize_struct(void *data, const data_fmt_desc_t *desc, const uint8_t *buf, uint32_t buf_len) {
    // Check arguments
    if ((data == NULL) || (desc == NULL) || (buf == NULL)) {
        return 0;
    }

    uint32_t bytes_read = 0;

    // Iterate over the fields
    for (uint8_t i = 0; i < desc->count; i++) {
        const data_fmt_field_desc_t *field_desc = &(desc->fields[i]);

        // Create pointer to data field
        uint8_t *field = (((uint8_t *)data) + field_desc->struct_offset);

        for (uint8_t j = 0; j < field_desc->array_len; j++) {
            // Deserialize the field
            uint32_t field_size = data_fmt_deserialize_field(field_desc->type, field, buf, (buf_len - bytes_read));

            if (field_size == 0) {
                return 0;
            }

            // Update vars (the serialized and deserialized representation of the data are assumed to be the same size)
            field += field_size;
            buf += field_size;
            bytes_read += field_size;
        }
    }

    return bytes_read;
}

/**
 * @brief Serialize a data value to a byte array
 *
 * @param[in]  type    Type of data
 * @param[in]  data    Pointer to the data. Must match the expected type based on the type parameter.
 * @param[out] buf     Pointer to the byte array for the serialized value
 * @param[in]  buf_len Length of the byte array
 *
 * @return Number of bytes produced in the serialization which must be equal to the size of the data object in memory.
 */
uint32_t data_fmt_serialize_field(data_fmt_field_type_t type, const void *data, uint8_t *buf, uint32_t buf_len) {
    if ((data == NULL) || (buf == NULL)) {
        return 0;
    }

    switch (type) {
    case DATA_FMT_FIELD_TYPE_U8: {
        if (buf_len >= sizeof(uint8_t)) {
            *buf = *(const uint8_t *)data;
            return sizeof(uint8_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U16: {
        if (buf_len >= sizeof(uint16_t)) {
            data_fmt_u16_to_arr_be(*(const uint16_t *)data, buf);
            return sizeof(uint16_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U32: {
        if (buf_len >= sizeof(uint32_t)) {
            data_fmt_u32_to_arr_be(*(const uint32_t *)data, buf);
            return sizeof(uint32_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U64: {
        if (buf_len >= sizeof(uint64_t)) {
            data_fmt_u64_to_arr_be(*(const uint64_t *)data, buf);
            return sizeof(uint64_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_F32: {
        if (buf_len >= sizeof(float32)) {
            data_fmt_f32_to_arr_be(*(const float32 *)data, buf);
            return sizeof(float32);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_F64: {
        if (buf_len >= sizeof(float64)) {
            data_fmt_f64_to_arr_be(*(const float64 *)data, buf);
            return sizeof(float64);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_BOOL: {
        if (buf_len >= sizeof(uint8_t)) {
            *buf = (uint8_t)(*(const bool *)data);
            return sizeof(uint8_t);
        }

        break;
    }

    default:
        break;
    }

    return 0;
}

/**
 * @brief Deserialize a data value from a byte array
 *
 * @param[in] type    Type of data
 * @param[in] data    Pointer to where the deserialized value will be placed.
 *                    Must match the expected type based on the type parameter.
 * @param[in] buf     Pointer to the byte array
 * @param[in] buf_len Length of the byte array
 *
 * @return Number of bytes consumed in the deserialization which must be equal to the size of the data object in memory.
 */
uint32_t data_fmt_deserialize_field(data_fmt_field_type_t type, void *data, const uint8_t *buf, uint32_t buf_len) {
    if ((data == NULL) || (buf == NULL)) {
        return 0;
    }

    switch (type) {
    case DATA_FMT_FIELD_TYPE_U8: {
        if (buf_len >= sizeof(uint8_t)) {
            *((uint8_t *)data) = *buf;
            return sizeof(uint8_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U16: {
        if (buf_len >= sizeof(uint16_t)) {
            *((uint16_t *)data) = data_fmt_arr_be_to_u16(buf);
            return sizeof(uint16_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U32: {
        if (buf_len >= sizeof(uint32_t)) {
            *((uint32_t *)data) = data_fmt_arr_be_to_u32(buf);
            return sizeof(uint32_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_U64: {
        if (buf_len >= sizeof(uint64_t)) {
            *((uint64_t *)data) = data_fmt_arr_be_to_u64(buf);
            return sizeof(uint64_t);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_F32: {
        if (buf_len >= sizeof(float32)) {
            *((float32 *)data) = data_fmt_arr_be_to_f32(buf);
            return sizeof(float32);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_F64: {
        if (buf_len >= sizeof(float64)) {
            *((float64 *)data) = data_fmt_arr_be_to_f64(buf);
            return sizeof(float64);
        }

        break;
    }

    case DATA_FMT_FIELD_TYPE_BOOL: {
        if (buf_len >= sizeof(uint8_t)) {
            *(bool *)data = (bool)(*buf);
            return sizeof(uint8_t);
        }

        break;
    }

    default:
        break;
    }

    return 0;
}
