/**
 * @file sequence.h
 * @brief Sequence utils
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Data-structure for holding information about a sequence.
 *
 * This structure must be initialized by sequence_init before it can be used
 * with any other operations.
 *
 * The active and empty slots are tracked using two linked lists:
 *   - active_list_head is the head of the active items
 *   - empty_list_head is the head of the empty items
 *
 * Since a slot can never be both active and empty the two lists are fully
 * independent and so the links for both lists can be stored in a single array
 * with size elements (the links field).
 */
typedef struct {
    /**
     * @brief Maximum number of items in the sequence. Must be > 0.
     */
    const uint8_t size;

    /**
     * @brief Function to store an item into the underlying storage buffer
     * at the specified index
     */
    void (*store_item)(const void *item, uint8_t storage_idx);

    /**
     * @brief Function used to determine where an item should be inserted
     * in the sequence.
     *
     * Return true if the item at idx_a goes before the item at idx_b.
     * Otherwise return false.
     */
    bool (*goes_before)(uint8_t idx_a, uint8_t idx_b);

    /**
     * @brief Pointer to an array of at least size numbers.
     *
     * Each number represents the index of the next item in the sequence
     */
    uint8_t *links;

    /**
     * @brief Index of the first active item in the sequence.
     *
     * Essentially the head of the populated linked list.
     */
    uint8_t active_list_head;

    /**
     * @brief Index of the first empty item in the sequence.
     *
     * Essentially the head of the empty linked list.
     */
    uint8_t empty_list_head;
} sequence_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void sequence_init(sequence_t *seq);
bool sequence_has_space(const sequence_t *seq);
bool sequence_is_empty(const sequence_t *seq);
bool sequence_insert(sequence_t *seq, const void *item);

uint8_t sequence_peek_first(const sequence_t *seq);
bool sequence_pop_first(sequence_t *seq);

#endif // SEQUENCE_H_
