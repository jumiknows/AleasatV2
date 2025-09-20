/*
 * @file mr26h256.c
 * @brief Driver for the MR25H256ACDF MRAM device
 */

#ifndef MR25H256_H_
#define MR25H256_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
// #include "obc_mram.h" Not needed since we don't have a mock
#include <stdbool.h>
#include "obc_error.h"
#include "obc_mram.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Largest accessible address in MRAM.
 */
#define MR25H256_MRAM_MAX_ADDRESS ((uint16_t) 32767U)

/**
 * @brief The base address for the bootloader partition. The bootloader uses
 * the first 1/4 of the MRAM, so this is address 0.
 */
#define MR25H256_MRAM_BOOTLOADER_PARTITION_BASE 0U

/**
 * @brief The size of the bootloader partition. It is defined as 1/4 of the
 * MRAM's space.
 */
#define MR25H256_MRAM_BOOTLOADER_PARTITION_SIZE_BYTES 8192U

/**
 * @brief MIBSPI timeout for MT25QL flash driver
 */
#define MRAM_MIBSPI_TIMEOUT_MS 30

// Status register
#define STATUS_WIP 0x00 // Write in progress
#define STATUS_WEL 0x01 // Write enabled

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* User API */
mram_err_t mram_mr25h256_init(void);
mram_err_t mram_mr25h256_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data);
mram_err_t mram_mr25h256_read(uint16_t addr, uint16_t size_bytes, uint8_t *data);
mram_err_t mram_mr25h256_read_status(uint8_t *reg_data);
mram_err_t mram_mr25h256_write_status(const uint8_t reg_data);
void mram_mr25h256_reset(void);

#endif
