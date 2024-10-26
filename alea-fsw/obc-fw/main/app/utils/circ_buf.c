/**
 * @file circ_buf.c
 * @brief Circular buffer implementation
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "circ_buf.h"

// Utils
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ITEM_PTR(_circ, _idx) (((uint8_t *)((_circ)->buf)) + ((_idx) * (_circ)->item_size))

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Insert a new item into the circular buffer, overwriting an old item
 * if necessary.
 *
 * @param[in] circ_buf Pointer to the circ_buf_t struct
 * @param[in] item     Pointer to the item to insert. This must point to at least circ_buf->item_size bytes of memory.
 *
 * @return false if the any of the arguments are NULL, otherwise true
 */
bool circ_buf_insert(circ_buf_t *circ_buf, const void *item) {
    if ((circ_buf == NULL) || (item == NULL)) {
        return false;
    }

    void *dest = ITEM_PTR(circ_buf, circ_buf->idx);
    memcpy(dest, item, circ_buf->item_size);
    circ_buf->idx = (circ_buf->idx + 1) % (circ_buf->count);

    return true;
}

/**
 * @brief Copy the most recent data in the circular buffer to a destination.
 * The data will be copied in the correct order (with the oldest item at the lowest address
 * and the newest item at the highest address).
 *
 * @param[in] circ_buf  Pointer to the circ_buf_t struct
 * @param[in] dest      Pointer to the copy destination. This must point to at least (circ_buf->item_size * num_items)
 *                      bytes of memory.
 * @param[in] num_items The number of items to copy. If a value larger than circ_buf->count is passed, it will be treated
 *                      as equal to circ_buf->count.
 *
 * @return The actual number of items copied out. If either pointer argument is NULL or num_items is 0, then 0 will be returned.
 */
uint32_t circ_buf_copy_newest(circ_buf_t *circ_buf, void *dest, uint32_t num_items) {
    if ((circ_buf == NULL) || (dest == NULL) || (num_items == 0)) {
        return 0;
    }

    // Saturate number items (upper limit)
    num_items = MIN(num_items, circ_buf->count);

    // Determine the index of the first item to copy
    uint32_t start_idx = (circ_buf->idx + circ_buf->count - num_items) % circ_buf->count;

    void *copy_src;
    void *copy_dest;
    uint32_t copy_num_items;
    uint32_t copy_size;

    // Copy from the first item up to (at most) the end of the underlying buffer
    copy_src       = ITEM_PTR(circ_buf, start_idx);
    copy_dest      = dest;
    copy_num_items = MIN(num_items, (circ_buf->count - start_idx));
    copy_size      = copy_num_items * circ_buf->item_size;
    memcpy(copy_dest, copy_src, copy_size);

    if (copy_num_items < num_items) {
        // Copy remaining items from the start of the underlying buffer up to (at most) the current index
        copy_src       = ITEM_PTR(circ_buf, 0);
        copy_dest      = ((uint8_t *)dest) + copy_size;
        copy_num_items = num_items - copy_num_items;
        copy_size      = copy_num_items * circ_buf->item_size;
        memcpy(copy_dest, copy_src, copy_size);
    }

    return num_items;
}
