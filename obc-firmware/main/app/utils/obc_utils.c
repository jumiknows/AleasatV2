/**
 * @file obc_utils.c
 * @brief General OBC utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_utils.h"

// OBC
#include "obc_hardwaredefs.h"
#include "gio.h"
#include "reg_system.h"

// FreeRTOS
#include "rtos.h"

// Standard library
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
int32_t cseq_to_num(char* seq) {
    char* str = NULL;
    uint32_t res = strtoul(seq, &str, 10);

    if(seq == str) {
        return -1;
    }
    else {
        return res;
    }
}

// ---------------------------------- TIMING -----------------------------------

/**
 * @brief simply loops for as long as you want doing nothing
 *
 * @param[in] ticks_to_wait: number of ticks to loop for
 * 
 * @warning TODO this will not work under optimization (dead code elimination)
 *          Replace all usages with either asm_busy_wait or obc_delay_us
 */
void busy_wait(uint32_t ticks_to_wait) {
    uint32_t i = 0;
    while (i <= ticks_to_wait) {
        i++;
    }
}

/**
 * @brief Generic delay function with 1us/tick resolution
 *
 * @param us Number of us to delay
 * @note The maximum allowed delay time for this function is 537 seconds
*/
void obc_delay_us(uint32_t us)
{
    uint32_t init_time = portCPU_CLOCK_US();

    while ((portCPU_CLOCK_US() - init_time) > us) {
    }
}

// ---------------------------------- SYSTEM -----------------------------------

/**
 * @brief Immediate software reset
 */
void restart_software(void) {
    systemREG1->SYSECR = systemREG1->SYSECR | ((uint32_t)1U << 15);
}
