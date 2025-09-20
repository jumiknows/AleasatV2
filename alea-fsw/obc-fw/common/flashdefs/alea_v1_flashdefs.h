/**
 * @file alea_v1_flashdefs.h
 * @brief Hardware definitions for ALEASAT OBC v1 external flash.
*/

#ifndef ALEA_V1_FLASHDEFS_H_
#define ALEA_V1_FLASHDEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// HAL
#include "reg_gio.h"
#include "reg_mibspi.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Pin numbers for when MIBSPI pins are used as GIOs
 */
#define MIBSPI_PIN_ENA      8

/**
 * @brief Flash MIBSPI configuration.
 */
#define FLASH_MIBSPI_REG    mibspiREG5
#define FLASH_DATA_FORMAT   0
#define FLASH_1_BYTE_GROUP  0 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP  1 // transfer group with 2 byte length
#define FLASH_5_BYTE_GROUP  2 // transfer group with 5 byte length
#define FLASH_20_BYTE_GROUP 3 // transfer group with 20 byte length
#define FLASH_69_BYTE_GROUP 4 // transfer group with 69 byte length

#define FLASH_CS0_PORT mibspiPORT5
#define FLASH_CS0_PIN  MIBSPI_PIN_ENA

/*
 * @brief Flash size configuration
 * @brief Flash size configuration - 32MB = 256Mb
 */
#define EXT_FLASH_BLOCK_SIZE  4096
#define EXT_FLASH_BLOCK_COUNT 8192

#endif // ALEA_V1_FLASHDEFS_H_
