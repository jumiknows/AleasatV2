/**
 * @file test_sequence.c
 * @brief Unit tests for sequence.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"
#include "sequence.h"

// Standard Library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define SEQUENCE_LEN 5U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void store_item(const void *item, uint8_t storage_idx);
static bool goes_before(uint8_t idx_a, uint8_t idx_b);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static const void *seq_storage[SEQUENCE_LEN] = { 0 };
static uint8_t seq_links[SEQUENCE_LEN] = { 0 };

sequence_t seq = {
    .size            = SEQUENCE_LEN,
    .store_item      = &store_item,
    .goes_before     = &goes_before,
    .links           = seq_links,
    .active_list_head   = 0,
    .empty_list_head = 0,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
    memset(seq_storage, 0, sizeof(seq_storage));
    sequence_init(&seq);
}

void tearDown(void) {
}

// hasSpace tests
void test_hasSpace_empty() {
    TEST_ASSERT_TRUE(sequence_has_space(&seq));
}

void test_hasSpace_1item() {
    sequence_insert(&seq, (const void *)1);

    TEST_ASSERT_TRUE(sequence_has_space(&seq));
}

void test_hasSpace_2items() {
    sequence_insert(&seq, (const void *)1);
    sequence_insert(&seq, (const void *)2);

    TEST_ASSERT_TRUE(sequence_has_space(&seq));
}

void test_hasSpace_full() {
    for (size_t i = 0; i < SEQUENCE_LEN; i++) {
        sequence_insert(&seq, (const void *)i);
    }

    TEST_ASSERT_FALSE(sequence_has_space(&seq));
}

// isEmpty tests
void test_isEmpty_empty() {
    TEST_ASSERT_TRUE(sequence_is_empty(&seq));
}

void test_isEmpty_1item() {
    sequence_insert(&seq, (const void *)1);

    TEST_ASSERT_FALSE(sequence_is_empty(&seq));
}

void test_isEmpty_2items() {
    sequence_insert(&seq, (const void *)1);
    sequence_insert(&seq, (const void *)2);

    TEST_ASSERT_FALSE(sequence_is_empty(&seq));
}

void test_isEmpty_full() {
    for (size_t i = 0; i < SEQUENCE_LEN; i++) {
        sequence_insert(&seq, (const void *)i);
    }

    TEST_ASSERT_FALSE(sequence_is_empty(&seq));
}

// Insert tests
void test_insert_1stItem() {
    TEST_ASSERT_TRUE(sequence_insert(&seq, (const void *)1));
}

void test_insert_2ndItem() {
    sequence_insert(&seq, (const void *)1);
    TEST_ASSERT_TRUE(sequence_insert(&seq, (const void *)2));
}

void test_insert_lastItem() {
    for (size_t i = 0; i < (SEQUENCE_LEN - 1); i++) {
        sequence_insert(&seq, (const void *)i);
    }
    TEST_ASSERT_TRUE(sequence_insert(&seq, (const void *)(SEQUENCE_LEN - 1)));
}

void test_insert_pastFull() {
    for (size_t i = 0; i < SEQUENCE_LEN; i++) {
        sequence_insert(&seq, (const void *)i);
    }
    TEST_ASSERT_FALSE(sequence_insert(&seq, (const void *)SEQUENCE_LEN));
}

// Pop First tests
void test_popFirst_empty() {
    TEST_ASSERT_FALSE(sequence_pop_first(&seq));
}

void test_popFirst_notEmpty() {
    sequence_insert(&seq, (const void *)1);
    TEST_ASSERT_TRUE(sequence_pop_first(&seq));
}

// Full sequence tests
void test_sequence_1item() {
    sequence_insert(&seq, (const void *)1);

    TEST_ASSERT_EQUAL(1, seq_storage[sequence_peek_first(&seq)]);
}

void test_sequence_2items_asc() {
    sequence_insert(&seq, (const void *)2);
    sequence_insert(&seq, (const void *)3);

    TEST_ASSERT_EQUAL(2, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(3, seq_storage[sequence_peek_first(&seq)]);
}

void test_sequence_2items_desc() {
    sequence_insert(&seq, (const void *)3);
    sequence_insert(&seq, (const void *)2);

    TEST_ASSERT_EQUAL(2, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(3, seq_storage[sequence_peek_first(&seq)]);
}

void test_sequence_full() {
    sequence_insert(&seq, (const void *)4);
    sequence_insert(&seq, (const void *)2);
    sequence_insert(&seq, (const void *)1);
    sequence_insert(&seq, (const void *)5);
    sequence_insert(&seq, (const void *)3);

    TEST_ASSERT_EQUAL(1, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(2, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(3, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(4, seq_storage[sequence_peek_first(&seq)]);
    sequence_pop_first(&seq);
    TEST_ASSERT_EQUAL(5, seq_storage[sequence_peek_first(&seq)]);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void store_item(const void *item, uint8_t storage_idx) {
    seq_storage[storage_idx] = item;
}

static bool goes_before(uint8_t idx_a, uint8_t idx_b) {
    return seq_storage[idx_a] < seq_storage[idx_b];
}
