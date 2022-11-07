/*
 * @file ecc_nor_flash.c
 * @brief Driver for the Cypress S25FL127S ECC nor flash
 * @author Julie Belleville (adapted from obc_flash, authored by Andrada Zoltan)
 * @date Feb 8, 2020
 *
 * Datasheet:
 * https://www.mouser.ca/datasheet/2/100/001-98282_S25FL127S_128_MBIT_16_MBYTE_3.0_V_SPI_FL-1299705.pdf
 */

#include "obc_ecc_flash.h"
#include "tms_mibspi.h"
#include "gio.h"

#ifdef FEATURE_ECC_FLASH

/**
 * Questions related to desired functionality:
 * 1. Do we want reset ability? (standby mode might affect current draw...)
 * 2. Do we need block protection at any point? Will have to clear those to erase or write
 */

// COMMANDS
#define ECC_WRITE_ENABLE  0x06
#define ECC_WRITE_DISABLE 0x04

#define ECC_READ_STATUS_REG_1 0x05
#define ECC_READ_STATUS_REG_2 0x07
#define ECC_READ_CONFIG_REG   0x35
#define ECC_READ_ECC_STATUS   0x18
#define ECC_WRITE             0x12
#define ECC_READ              0x13
#define ECC_READ_ID           0x9F
#define ECC_WRITE_REG         0x01

/**
 * @brief Timeout for MIBSPI mutex grab.
 */
#define MUTEX_TIMEOUT_MS 200

/**
 * @brief Maximum time an erase operation could take. See p.110 of datasheet
 * Max erase timeout based off of bulk erase time of 256 kB uniform sectors
 */
#define MAX_ERASE_TIMEOUT_S 200

/**
 * @brief Max time expected for a 256 byte page buffer write. Max value reported
 * in datasheet p.110 is 1.185ms, however this was seen to be too short in testing
 * so the timeout has been increased.
 */
#define MAX_WRITE_TIMEOUT_MS 10

mibspi_tg_t ecc_flash_1byte_tg = {.reg = ECC_FLASH_MIBSPI_REG, .transfer_group = ECC_FLASH_1_BYTE_GROUP, .cs_port = ECC_FLASH_CS0_PORT, .cs_pin = ECC_FLASH_CS0_PIN};

mibspi_tg_t ecc_flash_2bytes_tg = {.reg = ECC_FLASH_MIBSPI_REG, .transfer_group = ECC_FLASH_2_BYTE_GROUP, .cs_port = ECC_FLASH_CS0_PORT, .cs_pin = ECC_FLASH_CS0_PIN};

mibspi_tg_t ecc_flash_5bytes_tg = {.reg = ECC_FLASH_MIBSPI_REG, .transfer_group = ECC_FLASH_5_BYTE_GROUP, .cs_port = ECC_FLASH_CS0_PORT, .cs_pin = ECC_FLASH_CS0_PIN};

mibspi_tg_t ecc_flash_22bytes_tg = {.reg = ECC_FLASH_MIBSPI_REG, .transfer_group = ECC_FLASH_22_BYTE_GROUP, .cs_port = ECC_FLASH_CS0_PORT, .cs_pin = ECC_FLASH_CS0_PIN};

/* Private Function Prototypes -------------------------------*/
static flash_err_t ecc_flash_write_enable(bool enable);
static flash_err_t ecc_flash_is_busy(bool* busy);
static flash_err_t ecc_flash_read_register(uint8_t reg, uint8_t* reg_data);

/* API Functions ---------------------------------------------*/

/**
 * @ Brief Initialises ECC flash with block size of 256kB, page buffer wrap at 256 kB
 */
void ecc_flash_init(void) {
    // 0. Set reset pin high (not resetting)
    gioSetBit(ECC_FLASH_RST_PORT, ECC_FLASH_RST_PIN, 1);

    /*
     * 1. Check device ID
     * Read device ID command is structured as follows:
     *    command byte | 2 byte data buffer of device ID | 2 extraneous bytes
     */
    uint16_t tx_buffer[5] = {ECC_READ_ID, 0xFF, 0xFF, 0xFF, 0xFF};
    uint16_t rx_buffer[5] = {0};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Do a dummy transfer to allow for the MIBSPI peripheral to get to a good state
        uint16_t tx = 0xFF;
        mibspi_transmit(ecc_flash_1byte_tg, &tx);

        // Transmit the read status register command, and receive data
        mibspi_transmit_receive(ecc_flash_5bytes_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    }
    // Note: decided to only read Manufacturer ID (located at address 00h), and device ID (located at addresses 01h, 02h)
    // These two pieces of information should be sufficient to confirm successful read (and is a quicker check)
    if (!((rx_buffer[1] == 0x01) && (rx_buffer[2] == 0x20) && (rx_buffer[3] == 0x18))) {
        while (1) {
        } // loop so that system can easily be debugged
    }

    /**
     * 2. Set Block Erase Size to 256 kB uniform sectors
     *
     * Note that: The desired state of this bit (D8h_O) must be selected during the initial configuration
     * of the device during system manufacture - before the first program or erase operation on the main flash array is performed.
     * D8h_O must not be programmed after programming or erasing is done in the main flash array (datasheet)
     *
     */
    ecc_flash_write_register(ECC_STATUS_2_REG, 0x80); // 0b10000000, set block erase size only, keep others default
}

/**
 * @brief Program the ecc flash with data to store.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address to write to
 * @param size: Amount of data to write in bytes
 * @param data: Buffer storing the data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t ecc_flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    uint32_t idx = 0;
    while (idx < size_bytes) {
        // Set write enable
        if (ecc_flash_write_enable(TRUE) != FLASH_OK) {
            return FLASH_MIBSPI_ERR;
        }

        if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
            /* Transmit write command is structured as follows:
             *    command byte | 32-bit address | 17-byte data buffer
             */
            uint16_t tx_buffer[22] = {
                ECC_WRITE, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF};

            /* Calculate how much data to copy into the transmit buffer so that no more than
             * the passed buffer to the function is copied. If the size is not a multiple of 16,
             * the last transmission will be only partially filled with data and partially
             * filled with the value 0xFF, which does not change the value stored in flash.
             */
            uint8_t size_to_copy = ((idx + 17) > size_bytes) ? (size_bytes % 17) : 17;
            uint8_t i;
            for (i = 0; i < size_to_copy; i++) {
                tx_buffer[i + 5] = data[idx + i]; // + 5 to offset from the end of address
            }

            // Write to the flash
            err = mibspi_transmit(ecc_flash_22bytes_tg, tx_buffer);
            if (err != MIBSPI_NO_ERR) {
                break;
            }

            // Increment to next write address and index
            addr += 17;
            idx += 17;

            // Poll until the write has been completed
            bool busy              = TRUE;
            flash_err_t err_flash  = FLASH_OK;
            uint8_t timeout_ctr_ms = 0;
            do {
                if (timeout_ctr_ms == MAX_WRITE_TIMEOUT_MS) {
                    xSemaphoreGive(xMibspiMutexHandle);
                    return FLASH_ERASE_TIMEOUT_ERR;
                }
                timeout_ctr_ms++;
                vTaskDelay(pdMS_TO_TICKS(1));
                err_flash = ecc_flash_is_busy(&busy);
            } while ((busy == TRUE) && (err_flash == FLASH_OK));

            xSemaphoreGive(xMibspiMutexHandle);
        } else {
            return FLASH_MIBSPI_MUTEX_GRAB_ERR;
        }
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read data from the ecc flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address to read from
 * @param size: Amount of data to read in bytes
 * @param data: Buffer where read data will be stored
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t ecc_flash_read(uint32_t addr, uint32_t size_bytes, uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        uint32_t idx = 0;
        while (idx < size_bytes) {
            /* Transmit read command is structured as follows:
             *    command byte | 32-bit address | 17-byte dummy data buffer
             */
            uint16_t tx_buffer[22] = {
                ECC_READ, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF};
            uint16_t rx_buffer[22] = {0};

            // Read the address in flash
            err = mibspi_transmit_receive(ecc_flash_22bytes_tg, tx_buffer, rx_buffer);

            // Copy the read data into the receive buffer
            uint8_t cpy_idx;
            uint8_t size_to_copy = ((idx + 17) > size_bytes) ? (size_bytes % 17) : 17;
            for (cpy_idx = 0; cpy_idx < size_to_copy; cpy_idx++) {
                data[cpy_idx + (uint8_t)idx] = rx_buffer[cpy_idx + 5]; // + 5 is offset due to command and address
            }

            // Increment to next read address and index
            addr += 17;
            idx += 17;
        }

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

flash_err_t ecc_hard_reset(void) {
    gioSetBit(ECC_FLASH_RST_PORT, ECC_FLASH_RST_PIN, 0); // reset is when RESET# driven low
    vTaskDelay(pdMS_TO_TICKS(1));                        // reset pulse min width is 200 ns, p. 32
    gioSetBit(ECC_FLASH_RST_PORT, ECC_FLASH_RST_PIN, 1);
    return FLASH_OK;
}

/**
 * @brief Erases the ecc flash (either full block erase, or 256kB erase)
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address within the sector/block that is to be erased
 * @param erase_size: Block size to erase. This flash can do 256kB, or full flash.
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t ecc_flash_erase(uint32_t addr, ecc_flash_erase_sz_t erase_size) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    if (ecc_flash_write_enable(TRUE) != FLASH_OK) {
        return FLASH_MIBSPI_ERR;
    }

    uint16_t tx_buffer[5] = {(uint16_t)erase_size, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        if (erase_size == ECC_FULL_CHIP) {
            err = mibspi_transmit(ecc_flash_1byte_tg, tx_buffer);
        } else { // 256 kB
            err = mibspi_transmit(ecc_flash_5bytes_tg, tx_buffer);
        }

        // Poll until the erase has been completed
        bool busy             = TRUE;
        flash_err_t err_flash = FLASH_OK;
        uint8_t timeout_ctr_s = 0;
        do {
            if (timeout_ctr_s == MAX_ERASE_TIMEOUT_S) {
                xSemaphoreGive(xMibspiMutexHandle);
                return FLASH_ERASE_TIMEOUT_ERR;
            }
            timeout_ctr_s++;
            vTaskDelay(pdMS_TO_TICKS(1000));
            err_flash = ecc_flash_is_busy(&busy);
        } while ((busy == TRUE) && (err_flash == FLASH_OK));
        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }
    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read from status register 1 (status register write disable, programming error, erase
 * error, block protection, write enable latch, write in progress)
 *
 * 	@pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called
 *
 * @param reg_data: Buffer to store returned data
 * @return FLASH_OK if no error, correct error code otherwise
 */
flash_err_t ecc_flash_read_status_register_1(uint8_t* reg_data) {
    return ecc_flash_read_register(ECC_READ_STATUS_REG_1, reg_data);
}

/**
 * @brief Read from status register 2 (block erase size, page buffer wrap, IO3 reset, erase suspend,
 * program suspend)
 *
 * 	@pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called
 *
 * @param reg_data: Buffer to store returned data
 * @return FLASH_OK if no error, correct error code otherwise
 */
flash_err_t ecc_flash_read_status_register_2(uint8_t* reg_data) {
    return ecc_flash_read_register(ECC_READ_STATUS_REG_2, reg_data);
}

/**
 * @brief Read from ECC status register (error in ECC, error in ECC unit data, ECC disabled)
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called
 *
 * @param ecc_unit_addr: Address of the ecc unit of which we want the status
 * @param reg_data: Buffer to store returned data
 * @return FLASH_OK if no error, correct error code otherwise
 */
flash_err_t ecc_flash_read_ecc_status_register(uint32_t ecc_unit_addr, uint8_t* reg_data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    /* Transmit read ecc status register command is structured as follows:
     *    command byte | 32-bit address | 1 dummy byte | 16-byte dummy data buffer
     */
    uint16_t tx_buffer[22] = {(uint16_t)ECC_READ_ECC_STATUS,
                              (ecc_unit_addr >> 24) & 0xFF,
                              (ecc_unit_addr >> 16) & 0xFF,
                              (ecc_unit_addr >> 8) & 0xFF,
                              ecc_unit_addr & 0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF,
                              0xFF};
    uint16_t rx_buffer[22] = {0};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Transmit the read status register command, and receive data
        err = mibspi_transmit_receive(ecc_flash_22bytes_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    // Copy the read data into the receive buffer (16 bytes)
    uint8_t idx;
    for (idx = 0; idx < 16; idx++) {
        reg_data[idx] = rx_buffer[idx + 6]; // + 6 is offset due to command, address, dummy byte
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read from config register (latency code, start of block protection (top or bottom),
 * 									 volatility of block protection, param sector location, I/O
 * mode, freeze). See p. 54
 *
 * 	@pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called
 *
 * @param reg_data: Buffer to store returned data
 * @return FLASH_OK if no error, correct error code otherwise
 */
flash_err_t ecc_flash_read_config_register(uint8_t* reg_data) {
    return ecc_flash_read_register(ECC_READ_CONFIG_REG, reg_data);
}

/**
 * @brief Write a register in the ecc flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @ Warning: not all bits of each register can be written to
 *
 * @param reg: Register to write
 * @param reg_data: the byte to write
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t ecc_flash_write_register(ecc_flash_reg_t reg, const uint8_t reg_data) {
    if (ecc_flash_write_enable(TRUE) != FLASH_OK) {
        return FLASH_MIBSPI_ERR;
    }
    mibspi_err_t err = MIBSPI_NO_ERR;

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Transmit the write register command and data to write
        if (reg == ECC_STATUS_1_REG) {
            uint16_t tx_buffer[2] = {(uint16_t)ECC_WRITE, (uint16_t)reg_data};
            err                   = mibspi_transmit(ecc_flash_2bytes_tg, tx_buffer);
        } else if (reg == ECC_CONFIG_REG) {
            uint16_t tx_buffer[5] = {(uint16_t)ECC_WRITE, 0xFF, (uint16_t)reg_data, 0xFF, 0xFF};
            err                   = mibspi_transmit(ecc_flash_5bytes_tg, tx_buffer);
        } else {
            uint16_t tx_buffer[5] = {(uint16_t)ECC_WRITE, 0xFF, 0xFF, (uint16_t)reg_data, 0xFF};
            err                   = mibspi_transmit(ecc_flash_5bytes_tg, tx_buffer);
        }
        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/* -------------------------PRIVATE HELPER FUNCTIONS
 * ---------------------------------------------------*/

/**
 * @brief Read a register in the ecc nor flash. Allowed registers are status 1, config, status 2
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg: Register to read
 * @param reg_data: Buffer to store returned data
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t ecc_flash_read_register(ecc_flash_reg_t reg, uint8_t* reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {(uint16_t)reg, 0xFF};
    uint16_t rx_buffer[2] = {0};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Transmit the read status register command, and receive data
        err = mibspi_transmit_receive(ecc_flash_2bytes_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    *reg_data = (uint8_t)rx_buffer[1];

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Checks if a write or erase is currently in progress.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param busy: Stores the status of the being busy
 * @return FLASH_OK if no error, error code otherwise
 */
static flash_err_t ecc_flash_is_busy(bool* busy) {
    uint16_t tx_buffer[2] = {ECC_READ_STATUS_REG_1, 0xFF};
    uint16_t rx_buffer[2] = {0};

    mibspi_err_t err = mibspi_transmit_receive(ecc_flash_2bytes_tg, tx_buffer, rx_buffer);

    if (((uint8_t)rx_buffer[1] & 0x01) == 0) {
        *busy = FALSE;
    } else {
        *busy = TRUE;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Enables writes to the ecc flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @warning Must be enabled before any writes are performed
 * or any erase is performed.
 *
 * @param enable: 1 if write enable, 0 if write disable
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t ecc_flash_write_enable(bool enable) {
    uint16_t tx_buffer[1];
    if (enable) {
        tx_buffer[0] = ECC_WRITE_ENABLE;
    } else {
        tx_buffer[0] = ECC_WRITE_DISABLE;
    }

    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        err = mibspi_transmit(ecc_flash_1byte_tg, tx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

#endif // FEATURE_ECC_FLASH
