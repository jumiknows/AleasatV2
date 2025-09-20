/**
 * @file mram_utils.c
 * @brief MRAM utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

#include "obc_mram.h"
#include "logger.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void increment_reset_counter(void) {
    uint32_t counter = 0xFFFFFFFF;

    // Read the current counter
    if (mram_read(MRAM_RESET_COUNTER_ADDR, sizeof(counter), (uint8_t *)&counter) != MRAM_OK) {
#if FEATURE_HW_MRAM
        LOG_RESET_COUNTER__READ_ERROR();
#endif
        return;
    }

    // If uninitialized, start from zero
    if (counter == 0xFFFFFFFF) {
        counter = 0;
    }

    counter++;

    // Write back the updated counter
    if (mram_write(MRAM_RESET_COUNTER_ADDR, sizeof(counter), (const uint8_t *)&counter) != MRAM_OK) {
        LOG_RESET_COUNTER__WRITE_ERROR();
    }
}
