/*
 * @file flash_mt25ql.h
 * @brief Driver for the MT25QL NOR Flash device.
 */

#ifndef FLASH_MT25QL_H_
#define FLASH_MT25QL_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_flash.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Flash Commands
#define FLASH_RESET_ENABLE          0x66 // Must be executed prior to FLASH_RESET
#define FLASH_RESET                 0x99 // Soft reset the flash
#define FLASH_RDID                  0x9F // Flash read product ID
//#define FLASH_WRITE_EXT_ADDR_REG    0xC5 // Write extended address register
//#define FLASH_READ_EXT_ADDR_REG     0xC8 // Read extended address register
#define FLASH_ENTER_4BYTE_ADDR_MODE 0xB7
#define FLASH_EXIT_4BYTE_ADDR_MODE  0xE9
#define FLASH_READ                  0x03
#define FLASH_READ_4B_ADDR          0x13 // Read with 4 byte address
#define WRITE_ENABLE                0x06 // Must be executed prior to FLASH_WRITE or any erase
#define WRITE_DISABLE               0x04
#define FLASH_WRITE                 0x02 // Page program
#define FLASH_WRITE_4B_ADDR         0x12 // Page program with 4 byte address
#define READ_STATUS_REG             0x05 // Read status register
#define WRITE_STATUS_REG            0x01 // Write status register
#define READ_FLAG_STATUS_REG        0x70 // Read flag status register for error bits
#define CLEAR_FLAG_STATUS_REG       0x50
#define SECTOR_ERASE_32KB           0x52
#define SECTOR_ERASE_4KB            0x20
#define SECTOR_ERASE                0xD8
#define SECTOR_ERASE_4KB_4B_ADDR    0x21 // Sector erase 4kB with 4 byte address
#define SECTOR_ERASE_4B_ADDR        0xDC // Sector erase with 4 byte address
#define BULK_ERASE                  0xC7
#define ENTER_DEEP_SLEEP            0xB9
#define EXIT_DEEP_SLEEP             0xAB

// Status register
#define STATUS_WIP 0x00 // Write in progress
#define STATUS_WEL 0x01 // Write enabled

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* Startup functions */
void mt25ql_init(void);

/* User API */
flash_err_t mt25ql_read_id(uint8_t *device_data);
flash_err_t mt25ql_read_status_register(uint8_t *reg_data);
flash_err_t mt25ql_read_flag_status_register(uint8_t *reg_data);
flash_err_t mt25ql_write_status_register(const uint8_t *reg_data);
flash_err_t mt25ql_erase(uint32_t addr, flash_erase_sz_t erase_size);
flash_err_t mt25ql_write_64(uint32_t addr, const uint8_t *data);
flash_err_t mt25ql_read_64(uint32_t addr, uint8_t *data);

#endif // FLASH_MT25QL_H_
