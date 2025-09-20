/**
 * @file asm_utils.h
 * @brief Assembly utility functions
 */

#ifndef ASM_UTILS_H_
#define ASM_UTILS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// NOTE: asm_utils.asm is copied to RAM in obc_main() and executed there to ensure timing requirements

/**
 * @brief Jump to an address via a direct program-counter load.
 *
 * A return is not possible from this call
 *
 * @param addr The target address
 */
void asm_jump_to_addr(uint32_t addr);

/**
 * @brief Busy wait in the simplest while loop (tested at 96MHz CPU clock to be 5 clock cycles per iteration of the loop)
 * This function is executed from RAM instead of Flash to meet timing requirements
 *
 * @param loops The number of loops to wait
 */
void asm_busy_wait(uint32_t loops);

#endif // ASM_UTILS_H_
