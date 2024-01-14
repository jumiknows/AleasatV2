/*
 * @file mr25h256.h
 * @brief Driver for the MR25H256ACDF MRAM device
 * @author Steven Richter
 * @date Mar 5, 2020
 *
 *      Need to create HALCoGEN transfer groups for SPI1 that make sense with the NVCT purpose.
 * Example from flash:
 *
 *      In HALCoGEN, 4 transfer groups were created. This is somewhat efficient, as we don't waste
 * time sending a ton of dummy bytes when we just need to send a single byte command, as we would
 * with only 1 large transfer group.
 *
 *      TG number   Number of bytes
 *      2           1
 *      3           2
 *      4           4
 *      5           20
 *
 */

#ifndef MR25H256_H_
#define MR25H256_H_

#include <stdbool.h>
#include "sys_common.h"
#include "obc_error.h"

/**
 * @brief Largest accessible address in MRAM.
 */
#define MR25H256_MRAM_MAX_ADDRESS 32767U

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
 * @brief The base address for the application partition.
 * The application may use the bottom three quarters of the MRAM. The bootloader
 * has the top quarter reserved.
 */
#define MR25H256_MRAM_APPLICATION_PARTITION_BASE 8192U

mram_err_t mram_mr25h256_init(void);
mram_err_t mram_mr25h256_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data);
mram_err_t mram_mr25h256_read(uint16_t addr, uint16_t size_bytes, uint8_t *data);
mram_err_t mram_mr25h256_read_status(uint8_t *reg_data);
mram_err_t mram_mr25h256_write_status(const uint8_t reg_data);
void mram_mr25h256_reset(void);

#endif /* MR25H256_H_ */
