/**
 * @file boot_mt25ql.h
 * @brief Minimal driver for the MT25QL256 NOR Flash device for use in bootloader
 */

#ifndef BOOT_MT25QL_H_
#define BOOT_MT25QL_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Boot
#include "boot_error.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Flash Commands
#define FLASH_RDID                  0x9F // Flash read product ID
#define FLASH_READ_4B_ADDR          0x13 // Read with 4 byte address
#define FLASH_ENTER_4BYTE_ADDR_MODE 0xB7

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

boot_err_t boot_mt25ql_init(void);
boot_err_t boot_mt25ql_read_64(uint32_t addr, uint8_t *data);

#endif // BOOT_MT25QL_H_
