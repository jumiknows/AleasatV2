/**
 * @file circ_buf.h
 * @brief Circular buffer implementation
 */

#ifndef CIRC_BUF_H_
#define CIRC_BUF_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    void *buf;
    uint32_t item_size;
    uint32_t count;

    uint32_t idx;
} circ_buf_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

bool circ_buf_insert(circ_buf_t *circ_buf, const void *item);
uint32_t circ_buf_copy_newest(circ_buf_t *circ_buf, void *dest, uint32_t num_items);

#endif // CIRC_BUF_H_
