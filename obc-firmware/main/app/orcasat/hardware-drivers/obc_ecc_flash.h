/*
 * @file ecc_nor_flash.h
 * @brief Driver for the Cypress S25FL127S ECC nor flash
 * @author Julie Belleville
 * @date Feb 8, 2020
 */
#ifndef ORCASAT_HARDWARE_DRIVERS_OBC_ECC_FLASH_H_
#define ORCASAT_HARDWARE_DRIVERS_OBC_ECC_FLASH_H_

#include "obc_flash.h"

// Erase commands
#define ECC_BULK_ERASE   0x60
#define ECC_ERASE_SECTOR 0xDC

// Registers to read from
typedef enum ecc_flash_reg { ECC_STATUS_1_REG = 0, ECC_CONFIG_REG = 1, ECC_STATUS_2_REG = 2 } ecc_flash_reg_t;

/**
 * @brief Erasable sizes for the ecc flash.
 *
 */
typedef enum ecc_flash_erase_sz {
    /**
     * @brief Erases the full flash device.
     */
    ECC_FULL_CHIP = ECC_BULK_ERASE,

    /**
     * @brief Erases a 256KB sector.
     */
    ECC_SECTOR_256K = ECC_ERASE_SECTOR,
} ecc_flash_erase_sz_t;

void ecc_flash_init(void);

flash_err_t ecc_flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t *data);
flash_err_t ecc_flash_read(uint32_t addr, uint32_t size_bytes, uint8_t *data);
flash_err_t ecc_flash_erase(uint32_t addr, ecc_flash_erase_sz_t erase_size);

flash_err_t ecc_flash_read_status_register_1(uint8_t *reg_data);
flash_err_t ecc_flash_read_status_register_2(uint8_t *reg_data);
flash_err_t ecc_flash_read_ecc_status_register(uint32_t ecc_unit_addr, uint8_t *reg_data);
flash_err_t ecc_flash_read_config_register(uint8_t *reg_data);

flash_err_t ecc_flash_write_register(ecc_flash_reg_t reg, const uint8_t reg_data);

flash_err_t ecc_hard_reset(void);

#endif /* ORCASAT_HARDWARE_DRIVERS_OBC_ECC_FLASH_H_ */
