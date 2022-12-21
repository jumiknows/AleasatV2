/**
 * @file sequence.c
 * @brief Sequence utils
 * 
 * A sequence represents a sorted list of items. The underlying implementation
 * is similar to a linked list which allows efficient insertion of items anywhere
 * in the list.
 * 
 * The implementation is designed for statically allocated list items.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "sequence.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Flag to indicate that an item has no next in the linked list
 */
#define NULL_IDX(seq) ((seq)->size)

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize a sequence
 * 
 * This must be called before any other operations on the sequence.
 * 
 * After initialization, all of the items belong to the empty linked list
 * in the sequence 0 -> 1 -> 2 etc. and none of the items belong to the active
 * linked list.
 * 
 * @param[in] seq Pointer to sequence struct
 */
void sequence_init(sequence_t *seq) {
    uint32_t i = 0;
    for (i = 0; i < (seq->size - 1); i++) {
        seq->links[i] = i + 1;
    }
    seq->links[i] = NULL_IDX(seq);

    seq->active_list_head = NULL_IDX(seq);
    seq->empty_list_head = 0;
}

/**
 * @brief Checks if there is space to store one more item in the sequence.
 * 
 * @param[in] seq Pointer to the sequence struct
 * 
 * @return true if there is space, otherwise false
 */
bool sequence_has_space(const sequence_t *seq) {
    return (seq->empty_list_head != NULL_IDX(seq));
}

/**
 * @brief Checks if the sequence is empty
 * 
 * @param[in] seq Pointer to the sequence struct
 * 
 * @return true if the sequence is empty, otherwise false
 */
bool sequence_is_empty(const sequence_t *seq) {
    return (seq->active_list_head == NULL_IDX(seq));
}

/**
 * @brief Insert an item into the sequence if there is space.
 * 
 * Storing the item in the underlying data buffer will be handled by
 * calling store_item on the sequence.
 * 
 * Positioning the item in the sequence will be handled by iterating
 * over the sequence and calling goes_before for each item.
 * 
 * This is a worst case O(n) operation where n is the current size of the sequence.
 * 
 * @param[in] seq  Pointer to the sequence struct
 * @param[in] item Pointer to the item to insert
 * 
 * @return true if there was space to insert the item, otherwise false
 */
bool sequence_insert(sequence_t *seq, const void *item) {
    if (!sequence_has_space(seq)) {
        return false;
    }

    // Store the item
    uint8_t idx = seq->empty_list_head;
    seq->store_item(item, idx);

    // Update empty_list_head
    seq->empty_list_head = seq->links[idx];

    // Search for insertion point
    uint32_t prev_search_idx = NULL_IDX(seq);    // Index of the item AFTER which we should insert
    uint32_t search_idx = seq->active_list_head; // Index of the item BEFORE which we should insert

    while (search_idx != NULL_IDX(seq)) {
        if (seq->goes_before(idx, search_idx)) {
            break;
        }

        prev_search_idx = search_idx;
        search_idx = seq->links[search_idx];
    }

    if (prev_search_idx == NULL_IDX(seq)) {
        // Insert at beginning?
        seq->active_list_head = idx;
        seq->links[idx] = search_idx;
    } else {
        // Insert in middle or end
        seq->links[prev_search_idx] = idx;
        seq->links[idx] = search_idx;
    }

    return true;
}

/**
 * @brief Retrieves the index of the item currently at the head
 * of the populated list.
 * 
 * This operation is O(1).
 * 
 * @param[in] seq Pointer to the sequence struct
 * 
 * @return Index of the first item in the sequence
 */
uint8_t sequence_peek_first(const sequence_t *seq) {
    return seq->active_list_head;
}

/**
 * @brief Advance to the next item in the sequence.
 * 
 * The current head of the active list will be removed and the second
 * item will become the new head.
 * 
 * The previous active head will become the new head of the empty list
 * and will point to the previous head of the empty list.
 * 
 * This operation is O(1).
 * 
 * @param[in] seq Pointer to the sequence struct
 * 
 * @return true if the sequence can advance (i.e. is not empty), false if the sequence is empty
 */
bool sequence_pop_first(sequence_t *seq) {
    if (sequence_is_empty(seq)) {
        return false;
    }

    // Move pointer to the next item
    uint8_t popped = seq->active_list_head;
    seq->active_list_head = seq->links[popped];

    // The popped position is now empty
    // Insert it at the beginning of the empty chain
    seq->links[popped] = seq->empty_list_head;
    seq->empty_list_head = popped;

    return true;
}
