/**
 * @file test_circ_buf.c
 * @brief Unit tests for circ_buff.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "circ_buf.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CIRC_BUF_COUNT 5U

#define LEN(array)       (sizeof((array)) / sizeof((array)[0]))

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void fillUpTo(uint32_t n);
static void checkCopied(uint32_t *items_out, uint32_t num_items, uint32_t start);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static uint32_t buf[CIRC_BUF_COUNT] = { 0 };

static circ_buf_t circ_buf = {
    .buf       = buf,
    .item_size = sizeof(buf[0]),
    .count     = CIRC_BUF_COUNT,
    .idx       = 0
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
    memset(buf, 0, sizeof(buf));
    circ_buf.idx = 0;
}

void tearDown(void) {
}

// Insert Tests

void test_insert_bothNull(void) {
    bool success = circ_buf_insert(NULL, NULL);
    TEST_ASSERT_FALSE(success);
}

void test_insert_nullCircBuf(void) {
    uint32_t item = 0;
    bool success = circ_buf_insert(NULL, &item);
    TEST_ASSERT_FALSE(success);
}

void test_insert_nullItem(void) {
    bool success = circ_buf_insert(&circ_buf, NULL);
    TEST_ASSERT_FALSE(success);
}

void test_insert_oneItem(void) {
    uint32_t item = 1;
    bool success = circ_buf_insert(&circ_buf, &item);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL(item, buf[0]);
}

void test_insert_twoItems(void) {
    for (uint32_t i = 1; i <= 2; i++) {
        bool success = circ_buf_insert(&circ_buf, &i);
        TEST_ASSERT_TRUE(success);
    }

    for (uint32_t i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(i+1, buf[i]);
    }
}

void test_insert_fill(void) {
    for (uint32_t i = 1; i <= circ_buf.count; i++) {
        bool success = circ_buf_insert(&circ_buf, &i);
        TEST_ASSERT_TRUE(success);
    }

    for (uint32_t i = 0; i < circ_buf.count; i++) {
        TEST_ASSERT_EQUAL(i+1, buf[i]);
    }
}

void test_insert_fillPlusOne(void) {
    for (uint32_t i = 1; i <= (circ_buf.count + 1); i++) {
        bool success = circ_buf_insert(&circ_buf, &i);
        TEST_ASSERT_TRUE(success);
    }

    TEST_ASSERT_EQUAL((circ_buf.count + 1), buf[0]);
    for (uint32_t i = 1; i < circ_buf.count; i++) {
        TEST_ASSERT_EQUAL(i+1, buf[i]);
    }
}

void test_insert_fillTwicePlusOne(void) {
    for (uint32_t i = 1; i <= ((2*circ_buf.count) + 1); i++) {
        bool success = circ_buf_insert(&circ_buf, &i);
        TEST_ASSERT_TRUE(success);
    }

    TEST_ASSERT_EQUAL(((2*circ_buf.count) + 1), buf[0]);
    for (uint32_t i = 1; i < circ_buf.count; i++) {
        TEST_ASSERT_EQUAL((circ_buf.count + i+1), buf[i]);
    }
}

// Copy Newest Tests

void test_copyNewest_nullCircBuf(void) {
    uint32_t item_out = 0;
    uint32_t copied = circ_buf_copy_newest(NULL, &item_out, 1);
    TEST_ASSERT_EQUAL(0, copied);
}

void test_copyNewest_nullDest(void) {
    uint32_t copied = circ_buf_copy_newest(&circ_buf, NULL, 1);
    TEST_ASSERT_EQUAL(0, copied);
}

void test_copyNewest_zeroItems(void) {
    uint32_t item_out = 0;
    uint32_t copied = circ_buf_copy_newest(&circ_buf, &item_out, 0);
    TEST_ASSERT_EQUAL(0, copied);
}

void test_copyNewest_firstItem(void) {
    fillUpTo(1);

    uint32_t item_out = 0;
    uint32_t copied = circ_buf_copy_newest(&circ_buf, &item_out, 1);

    TEST_ASSERT_EQUAL(1, copied);
    TEST_ASSERT_EQUAL(1, item_out);
}

void test_copyNewest_firstItems(void) {
    fillUpTo(3);

    uint32_t items_out[3] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 1);
}

void test_copyNewest_middleItem(void) {
    fillUpTo(3);

    uint32_t item_out = 0;
    uint32_t copied = circ_buf_copy_newest(&circ_buf, &item_out, 1);

    TEST_ASSERT_EQUAL(1, copied);
    TEST_ASSERT_EQUAL(3, item_out);
}

void test_copyNewest_middleItems(void) {
    fillUpTo(4);

    uint32_t items_out[3] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 2);
}

void test_copyNewest_lastItem(void) {
    fillUpTo(circ_buf.count);

    uint32_t item_out = 0;
    uint32_t copied = circ_buf_copy_newest(&circ_buf, &item_out, 1);

    TEST_ASSERT_EQUAL(1, copied);
    TEST_ASSERT_EQUAL(5, item_out);
}

void test_copyNewest_lastItems(void) {
    fillUpTo(circ_buf.count);

    uint32_t items_out[3] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 3);
}

void test_copyNewest_wrapItems(void) {
    fillUpTo(circ_buf.count + 1);

    uint32_t items_out[3] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 4);
}

void test_copyNewest_allItemsAligned(void) {
    fillUpTo(circ_buf.count);

    uint32_t items_out[CIRC_BUF_COUNT] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 1);
}

void test_copyNewest_allItemsWrap(void) {
    fillUpTo(circ_buf.count + 2);

    uint32_t items_out[CIRC_BUF_COUNT] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, LEN(items_out));

    TEST_ASSERT_EQUAL(LEN(items_out), copied);
    checkCopied(items_out, LEN(items_out), 3);
}

void test_copyNewest_tooManyItems(void) {
    fillUpTo(circ_buf.count + 2);

    uint32_t items_out[CIRC_BUF_COUNT + 1] = { 0 };
    uint32_t copied = circ_buf_copy_newest(&circ_buf, items_out, (CIRC_BUF_COUNT + 1));

    TEST_ASSERT_EQUAL(CIRC_BUF_COUNT, copied);
    checkCopied(items_out, CIRC_BUF_COUNT, 3);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void fillUpTo(uint32_t n) {
    for (uint32_t i = 1; i <= n; i++) {
        circ_buf_insert(&circ_buf, &i);
    }
}

static void checkCopied(uint32_t *items_out, uint32_t num_items, uint32_t start) {
    for (uint32_t i = 0; i < num_items; i++) {
        TEST_ASSERT_EQUAL(i+start, items_out[i]);
    }
}
