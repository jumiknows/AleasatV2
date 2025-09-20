/**
 * @file obc_utils.c
 * @brief General OBC utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_utils.h"
#include "mram_utils.h"

// Standard library
#include <stdint.h>
#include <stdlib.h>

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

// ---------------------------- STRING MANIPULATION ----------------------------

/**
 * @brief converts a string to an unsigned integer and checks for error
 *
 * @param[in] seq: char sequence representing an integer MSB -> LSB
 * @return equivalent integer or -1 if there is an error
 */
int32_t cseq_to_num(char *seq) {
    char *str = NULL;
    uint32_t res = strtoul(seq, &str, 10);

    if (seq == str) {
        return -1;
    } else {
        return res;
    }
}

// ---------------------------------- TIMING -----------------------------------


/**
 * @brief Generic delay function with 1us/tick resolution
 *
 * @param us Number of us to delay
 * @note The maximum allowed delay time for this function is 537 seconds
*/
void obc_delay_us(uint32_t us) {
    uint32_t init_time = SYSTEM_TIME_US();

    while (true) {
        uint32_t now = SYSTEM_TIME_US();

        if ((now - init_time) >= us) {
            break;
        }
    }
}

void CPU_SOFT_RESET(void) {
    increment_reset_counter();

    vTaskDelay(pdMS_TO_TICKS(
                   100)); // This delay is needed to allow for the datalink layer to ACK an incoming reset command from obcpy, otherwise we'd enter a reset loop

    vPrivilegedCPUReset();
}
