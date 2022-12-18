/**
 * @file test_sequence.c
 * @brief Unit tests for sequence.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "io_stream.h"
#include "buffered_io.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define DATA_SOURCE_SIZE 32
#define DATA_DEST_SIZE   32

#define MAX_BLOCK_SIZE   8

#define BUF_IN_SIZE      8
#define BUF_OUT_SIZE     16

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void populate_array(uint8_t *data, uint32_t data_len, uint32_t start);
static void assert_data_zero(const uint8_t *data, uint32_t data_len);

static uint32_t read_block_data_source(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left);
static uint32_t write_data_dest(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Input
static uint8_t data_source[DATA_SOURCE_SIZE];
static uint32_t data_source_pos = 0;

static const buffered_block_istream_t data_input = {
    .handle         = NULL,
    .max_block_size = MAX_BLOCK_SIZE,
    .read_block     = &read_block_data_source,
};

static uint8_t buffered_input_buf[BUF_IN_SIZE];

buffered_input_t buffered_input = {
    .size = BUF_IN_SIZE,
    .buf = buffered_input_buf,
    .input = &data_input,

    .start = 0,
    .valid_len = 0,
};

static const io_istream_t input_stream = {
    .handle = &buffered_input,
    .read   = &buffered_io_read,
};

// Output
static uint8_t data_dest[DATA_DEST_SIZE];
static uint32_t data_dest_pos = 0;

static const io_ostream_t data_output = {
    .handle = NULL,
    .write  = &write_data_dest,
    .flush  = NULL,
};

static uint8_t buffered_output_buf[BUF_OUT_SIZE];

buffered_output_t buffered_output = {
    .size = BUF_OUT_SIZE,
    .buf = buffered_output_buf,
    .output = &data_output,

    .offset = 0,
};

static const io_ostream_t output_stream = {
    .handle = &buffered_output,
    .write  = &buffered_io_write,
    .flush  = &buffered_io_flush,
};

// Test variables
static uint32_t next_input_block_size = BUF_IN_SIZE; // This much data will be returned on the next call to read_block_data_source

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
    // Reset input
    populate_array(data_source, sizeof(data_source), 0);
    memset(buffered_input_buf, 0, sizeof(buffered_input_buf));
    data_source_pos = 0;
    buffered_io_input_reset(&buffered_input);
    next_input_block_size = BUF_IN_SIZE;

    // Reset output
    memset(data_dest, 0, sizeof(data_dest));
    memset(buffered_output_buf, 0, sizeof(buffered_output_buf));
    data_dest_pos = 0;
    buffered_io_output_reset(&buffered_output);
}

void tearDown(void) {
}

// Write tests

void test_write_nullHandle(void) {
    uint8_t data[BUF_OUT_SIZE / 4];
    populate_array(data, sizeof(data), 0);

    uint32_t bytes_written = output_stream.write(NULL, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(0, bytes_written);

    // Data should not be written yet
    assert_data_zero(data_dest, sizeof(data_dest));
}

void test_write_nullData(void) {
    uint32_t bytes_written = output_stream.write(output_stream.handle, NULL, (BUF_OUT_SIZE / 4), 0, NULL);
    TEST_ASSERT_EQUAL(0, bytes_written);

    // Data should not be written yet
    assert_data_zero(data_dest, sizeof(data_dest));
}

void test_write_partialBuf(void) {
    uint8_t data[BUF_OUT_SIZE / 4];
    populate_array(data, sizeof(data), 0);

    uint32_t bytes_written = output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_written);

    // Data should not be written yet
    assert_data_zero(data_dest, sizeof(data_dest));
}

void test_write_2partialBufs(void) {
    uint8_t data[BUF_OUT_SIZE / 4];

    for (uint32_t i = 0; i < 2; i++) {
        populate_array(data, sizeof(data), (i * sizeof(data)));

        uint32_t bytes_written = output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);
        TEST_ASSERT_EQUAL(sizeof(data), bytes_written);
    }

    // Data should not be written yet
    assert_data_zero(data_dest, sizeof(data_dest));
}

void test_write_fullBuf_1Shot(void) {
    uint8_t data[BUF_OUT_SIZE];
    populate_array(data, sizeof(data), 0);

    uint32_t bytes_written = output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_written);

    // Data should be written now
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, data_dest, sizeof(data));
    assert_data_zero(&(data_dest[sizeof(data)]), (sizeof(data_dest) - sizeof(data)));
}

void test_write_fullBuf_2Shots(void) {
    uint8_t data[BUF_OUT_SIZE / 2];

    for (uint32_t i = 0; i < 2; i++) {
        populate_array(data, sizeof(data), (i * sizeof(data)));

        uint32_t bytes_written = output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);
        TEST_ASSERT_EQUAL(sizeof(data), bytes_written);
    }

    // Data should be written now
    uint8_t *data_dest_ptr = data_dest;
    for (uint32_t i = 0; i < 2; i++) {
        populate_array(data, sizeof(data), (i * sizeof(data)));
        TEST_ASSERT_EQUAL_UINT8_ARRAY(data, data_dest_ptr, sizeof(data));
        data_dest_ptr += sizeof(data);
    }

    assert_data_zero(data_dest_ptr, (sizeof(data_dest) - (2 * sizeof(data))));
}

void test_write_fullBuf_overshoot(void) {
    uint8_t data[BUF_OUT_SIZE + 1];
    populate_array(data, sizeof(data), 0);
    
    uint32_t bytes_written = output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_written);

    // Data should be written now except last byte
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, data_dest, BUF_OUT_SIZE);
    assert_data_zero(&(data_dest[BUF_OUT_SIZE]), (sizeof(data_dest) - BUF_OUT_SIZE));
}

void test_write_fullBuf_overshoot_2Shots(void) {
    uint8_t data1[BUF_OUT_SIZE - 1];
    uint8_t data2[BUF_OUT_SIZE - 1];

    populate_array(data1, sizeof(data1), 0);
    uint32_t bytes_written = output_stream.write(output_stream.handle, data1, sizeof(data1), 0, NULL);

    populate_array(data2, sizeof(data2), sizeof(data1));
    bytes_written = output_stream.write(output_stream.handle, data2, sizeof(data2), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data2), bytes_written);

    // Data should be written now (for one buffer)
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data1, data_dest, sizeof(data1));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data2, &(data_dest[sizeof(data2)]), (BUF_OUT_SIZE - sizeof(data1)));
    assert_data_zero(&(data_dest[BUF_OUT_SIZE]), (sizeof(data_dest) - BUF_OUT_SIZE));
}

// Flush tests

void test_flush_nullHandle(void) {
    uint8_t data[BUF_OUT_SIZE / 4];
    populate_array(data, sizeof(data), 0);

    output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);

    bool success = output_stream.flush(NULL, 0, NULL);
    TEST_ASSERT_FALSE(success);

    assert_data_zero(data_dest, sizeof(data_dest));
}

void test_flush_partialBuf(void) {
    uint8_t data[BUF_OUT_SIZE / 4];
    populate_array(data, sizeof(data), 0);

    output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);

    bool success = output_stream.flush(output_stream.handle, 0, NULL);
    TEST_ASSERT_TRUE(success);

    // Data should be written now
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, data_dest, sizeof(data));
    assert_data_zero(&(data_dest[sizeof(data)]), (sizeof(data_dest) - sizeof(data)));
}

void test_flush_overshootBuf(void) {
    uint8_t data[BUF_OUT_SIZE + 1];
    populate_array(data, sizeof(data), 0);

    output_stream.write(output_stream.handle, data, sizeof(data), 0, NULL);

    bool success = output_stream.flush(output_stream.handle, 0, NULL);
    TEST_ASSERT_TRUE(success);

    // Data should be written now
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, data_dest, sizeof(data));
    assert_data_zero(&(data_dest[sizeof(data)]), (sizeof(data_dest) - sizeof(data)));
}

// Read tests

void test_read_nullHandle(void) {
    uint8_t data[BUF_IN_SIZE];

    uint32_t bytes_read = input_stream.read(NULL, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(0, bytes_read);
}

void test_read_nullData(void) {
    uint8_t data[BUF_IN_SIZE];

    uint32_t bytes_read = input_stream.read(input_stream.handle, NULL, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(0, bytes_read);
}

void test_read_bufTooSmall(void) {
    uint8_t data[BUF_IN_SIZE];

    buffered_input_t buffered_input = {
        .size = (BUF_IN_SIZE - 1),
        .buf = buffered_input_buf,
        .input = &data_input,

        .start = 0,
        .valid_len = 0,
    };

    uint32_t bytes_read = buffered_io_read(&buffered_input, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(0, bytes_read);
}

void test_read_partialBlock(void) {
    uint8_t data[BUF_IN_SIZE / 2];

    uint32_t bytes_read = input_stream.read(input_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_read);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source, data, sizeof(data));
}

void test_read_2partialBlocks(void) {
    uint8_t data[BUF_IN_SIZE / 4];

    const uint8_t *data_source_ptr = data_source;
    for (uint32_t i = 0; i < 2; i++) {
        uint32_t bytes_read = input_stream.read(input_stream.handle, data, sizeof(data), 0, NULL);
        TEST_ASSERT_EQUAL(sizeof(data), bytes_read);

        TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source_ptr, data, sizeof(data));

        data_source_ptr += sizeof(data);
    }
}

void test_read_fullBlock_1Shot(void) {
    uint8_t data[BUF_IN_SIZE];

    uint32_t bytes_read = input_stream.read(input_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_read);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source, data, sizeof(data));
}

void test_read_fullBlock_2Shots(void) {
    uint8_t data[BUF_IN_SIZE];

    input_stream.read(input_stream.handle, data, (sizeof(data) / 2), 0, NULL);

    uint32_t bytes_read = input_stream.read(input_stream.handle, &(data[(sizeof(data) / 2)]), (sizeof(data) / 2), 0, NULL);
    TEST_ASSERT_EQUAL((sizeof(data) / 2), bytes_read);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source, data, sizeof(data));
}

void test_read_overshootBlock(void) {
    uint8_t data[BUF_IN_SIZE + 1];

    uint32_t bytes_read = input_stream.read(input_stream.handle, data, sizeof(data), 0, NULL);
    TEST_ASSERT_EQUAL(sizeof(data), bytes_read);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source, data, sizeof(data));
}

void test_read_overshootBlock_2Shots(void) {
    uint8_t data[BUF_IN_SIZE + 2];

    input_stream.read(input_stream.handle, data, (sizeof(data) / 2), 0, NULL);

    uint32_t bytes_read = input_stream.read(input_stream.handle, &(data[(sizeof(data) / 2)]), (sizeof(data) / 2), 0, NULL);
    TEST_ASSERT_EQUAL((sizeof(data) / 2), bytes_read);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_source, data, sizeof(data));
}

// TODO ALEA-840 test error paths and timeouts

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void populate_array(uint8_t *data, uint32_t data_len, uint32_t start) {
    for (uint32_t i = 0; i < data_len; i++) {
        // Fill data source with an incrementing pattern
        data[i] = (start + i);
    }
}

static void assert_data_zero(const uint8_t *data, uint32_t data_len) {
    for (uint32_t i = 0; i < data_len; i++) {
        TEST_ASSERT_EQUAL(0, data[i]);
    }
}

static uint32_t read_block_data_source(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left) {
    if (buf_len < next_input_block_size) {
        TEST_FAIL_MESSAGE("Read out of bounds");
        return 0;
    }
    if ((data_source_pos + next_input_block_size) > sizeof(data_source)) {
        TEST_FAIL_MESSAGE("Read out of bounds");
        return 0;
    }

    memcpy(buf, &(data_source[data_source_pos]), next_input_block_size);
    if (timeout_left != NULL) {
        *timeout_left = 0;
    }

    data_source_pos += next_input_block_size;
    return next_input_block_size;
}

static uint32_t write_data_dest(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    if ((data_dest_pos + num_bytes) > sizeof(data_dest)) {
        TEST_FAIL_MESSAGE("Write out of bounds");
        return 0;
    }

    memcpy(&(data_dest[data_dest_pos]), data, num_bytes);
    if (timeout_left != NULL) {
        *timeout_left = 0;
    }

    data_dest_pos += num_bytes;
    return num_bytes;
}
