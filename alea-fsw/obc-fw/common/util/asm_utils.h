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

/**
 * @brief Jump to an address via a direct program-counter load.
 * 
 * A return is not possible from this call
 * 
 * @param addr The target address
 */
void asm_jump_to_addr(uint32_t addr);

/**
 * @brief Busy wait in the simplest while loop (approximately 5 cycles / loop)
 * 
 * @param loops The number of loops to wait
 */
void asm_busy_wait(uint32_t loops);

#endif // ASM_UTILS_H_
