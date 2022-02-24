/*
 * flash_mt25ql_defs.h
 *
 *  Created on: Apr. 22, 2020
 *      Author: Andrada Zoltan
 */

#ifndef FLASH_MT25QL_DEFS_H_
#define FLASH_MT25QL_DEFS_H_

// Flash Commands
#define FLASH_RESET_ENABLE    0x66 // Must be executed prior to FLASH_RESET
#define FLASH_RESET           0x99 // Soft reset the flash
#define FLASH_RDID            0x9F // Flash read product ID
#define FLASH_READ            0x03
#define WRITE_ENABLE          0x06 // Must be executed prior to FLASH_WRITE or any erase
#define WRITE_DISABLE         0x04
#define FLASH_WRITE           0x02 // Page program
#define READ_STATUS_REG       0x05 // Read status register
#define WRITE_STATUS_REG      0x01 // Write status register
#define READ_FLAG_STATUS_REG  0x70 // Read flag status register for error bits
#define CLEAR_FLAG_STATUS_REG 0x50
#define SECTOR_ERASE_32KB     0x52
#define SECTOR_ERASE_4KB      0x20
#define SECTOR_ERASE          0xD8
#define BULK_ERASE            0xC7
#define ENTER_DEEP_SLEEP      0xB9
#define EXIT_DEEP_SLEEP       0xAB

// Status register
#define STATUS_WIP 0x00 // Write in progress
#define STATUS_WEL 0x01 // Write enabled

#endif /* FLASH_MT25QL_DEFS_H_ */
