/**
 * @file flash_mt25ql.c
 * @brief Driver for the MT25QL NOR Flash device.
 * @author Andrada Zoltan
 * @date July 24, 2019
 *
 * 		Datasheet:
 * https://www.micron.com/-/media/client/global/documents/products/data-sheet/nor-flash/serial-nor/mt25q/die-rev-a/mt25q_qlhs_l_128_aba_0.pdf
 */

#ifdef PLATFORM_ORCA_V1

#include "flash_mt25ql.h"
#include "flash_mt25ql_defs.h"
#include "obc_mibspi.h"
#include "gio.h"

/**
 * @brief Timeout for MIBSPI mutex grab.
 */
#define MUTEX_TIMEOUT_MS 200

/**
 * @brief Maximum time an erase operation could take. See max bulk erase
 * time in datasheet on page 86.
 */
#define MAX_ERASE_TIMEOUT_MS (114 * 1000)

/**
 * @brief The time to allow for other tasks to run between checks for erase
 * completion.
 */
#define FLASH_ERASE_INCREMENT_MS 10

/**
 * @brief Maximum time a write operation could take for 256 bytes. See max
 * page program time in datasheet on page 86.
 */
#define MAX_WRITE_TIMEOUT_MS 2

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the flash chip.
 */
mibspi_tg_t flash_1byte_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_1_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

mibspi_tg_t flash_2bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_2_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

mibspi_tg_t flash_4bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_4_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

mibspi_tg_t flash_20bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_20_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

const uint8_t erase_cmds[] = {BULK_ERASE, SECTOR_ERASE, SECTOR_ERASE_32KB, SECTOR_ERASE_4KB};

/* Private Function Prototypes -----------------------------------------------*/
static flash_err_t mt25ql_is_busy(bool* busy);
static flash_err_t mt25ql_write_enable(bool enable);
static flash_err_t mt25ql_read_register(uint8_t reg, uint8_t* reg_data);
static flash_err_t mt25ql_write_register(uint8_t reg, const uint8_t* reg_data);

/* API Functions -------------------------------------------------------------*/

/**
 * @brief Initializes the flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 */
void mt25ql_init(void) {
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Do a dummy transfer to allow for the MIBSPI peripheral to get to a good state
        uint16_t tx = 0xFF;
        mibspi_transmit(flash_1byte_tg, &tx);
        xSemaphoreGive(xMibspiMutexHandle);
    }
}

/**
 * @brief Reads the status register.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg_data: buffer to store the read register value
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read_status_register(uint8_t* reg_data) {
    return mt25ql_read_register(READ_STATUS_REG, reg_data);
}

/**
 * @brief Reads the flag status register.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg_data: buffer to store the read register value
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read_flag_status_register(uint8_t* reg_data) {
    return mt25ql_read_register(READ_FLAG_STATUS_REG, reg_data);
}

/**
 * @brief Writes to the status register.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg_data: value to write to the status register
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_write_status_register(const uint8_t* reg_data) {
    return mt25ql_write_register(WRITE_STATUS_REG, reg_data);
}

/**
 * @brief Erases the flash in various sized chunks.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address within the sector/block that is to be erased, must be byte-aligned.
 * @param erase_size: Block size to erase. This flash can do 64KB, 32KB, 4KB, or full flash.
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_erase(uint32_t addr, flash_erase_sz_t erase_size) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    // Set write enable
    if (mt25ql_write_enable(TRUE) != FLASH_OK) {
        return FLASH_MIBSPI_ERR;
    }

    uint16_t tx_buffer[4] = {(uint16_t)erase_cmds[erase_size], (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Erase
        if (erase_size == FULL_CHIP) {
            err = mibspi_transmit(flash_1byte_tg, tx_buffer);
        } else {
            err = mibspi_transmit(flash_4bytes_tg, tx_buffer);
        }

        // Poll until the erase has been completed
        bool busy              = TRUE;
        flash_err_t err_flash  = FLASH_OK;
        uint8_t timeout_ctr_ms = 0;
        do {
            if (timeout_ctr_ms == MAX_ERASE_TIMEOUT_MS) {
                xSemaphoreGive(xMibspiMutexHandle);
                return FLASH_ERASE_TIMEOUT_ERR;
            }
            timeout_ctr_ms += FLASH_ERASE_INCREMENT_MS;
            vTaskDelay(pdMS_TO_TICKS(FLASH_ERASE_INCREMENT_MS));
            err_flash = mt25ql_is_busy(&busy);
        } while ((busy == TRUE) && (err_flash == FLASH_OK));

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Program the flash with data to store.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address to write to, must be byte-aligned
 * @param size: Amount of data to write in bytes
 * @param data: Buffer storing the data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_write(uint32_t addr, uint32_t size_bytes, const uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    uint32_t idx = 0;
    while (idx < size_bytes) {
        // Set write enable
        if (mt25ql_write_enable(TRUE) != FLASH_OK) {
            return FLASH_MIBSPI_ERR;
        }

        if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
            /* Transmit write command is structured as follows:
             *    command byte | 24-bit address | 16-byte data buffer
             */
            uint16_t tx_buffer[20] = {FLASH_WRITE, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                      0xFF};

            /* Calculate how much data to copy into the transmit buffer so that no more than
             * the passed buffer to the function is copied. If the size is not a multiple of 16,
             * the last transmission will be only partially filled with data and partially
             * filled with the value 0xFF, which does not change the value stored in flash.
             */
            uint8_t size_to_copy = ((idx + 16) > size_bytes) ? (size_bytes % 16) : 16;
            uint8_t i;
            for (i = 0; i < size_to_copy; i++) {
                tx_buffer[i + 4] = data[idx + i];
            }

            // Write to the flash
            err = mibspi_transmit(flash_20bytes_tg, tx_buffer);
            if (err != MIBSPI_NO_ERR) {
                break;
            }

            // Increment to next write address and index
            addr += 16;
            idx += 16;

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
                err_flash = mt25ql_is_busy(&busy);
            } while ((busy == TRUE) && (err_flash == FLASH_OK));

            xSemaphoreGive(xMibspiMutexHandle);
        } else {
            return FLASH_MIBSPI_MUTEX_GRAB_ERR;
        }
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read data from the flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param addr: Address to read from, must be byte-aligned
 * @param size: Amount of data to read in bytes
 * @param data: Buffer where read data will be stored
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read(uint32_t addr, uint32_t size_bytes, uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        uint32_t idx = 0;
        while (idx < size_bytes) {
            /* Transmit read command is structured as follows:
             *    command byte | 24-bit address | 16-byte dummy data buffer
             */
            uint16_t tx_buffer[20] = {FLASH_READ, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            uint16_t rx_buffer[20] = {0};

            // Read the address in flash
            err = mibspi_transmit_receive(flash_20bytes_tg, tx_buffer, rx_buffer);

            // Copy the read data into the receive buffer
            uint8_t cpy_idx;
            uint8_t size_to_copy = ((idx + 16) > size_bytes) ? (size_bytes % 16) : 16;
            for (cpy_idx = 0; cpy_idx < size_to_copy; cpy_idx++) {
                data[cpy_idx + (uint8_t)idx] = rx_buffer[cpy_idx + 4];
            }

            // Increment to next read address and index
            addr += 16;
            idx += 16;
        }

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/* PRIVATE HELPER FUNCTIONS ---------------------------------------------------*/

/**
 * @brief Checks if a write or erase is currently in progress.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param busy: Stores the status of the being busy
 * @return FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_is_busy(bool* busy) {
    uint16_t tx_buffer[2] = {READ_FLAG_STATUS_REG, 0xFF};
    uint16_t rx_buffer[2] = {0};

    mibspi_err_t err = mibspi_transmit_receive(flash_2bytes_tg, tx_buffer, rx_buffer);

    if (((uint8_t)rx_buffer[1] & 0x80) == 0) {
        *busy = TRUE;
    } else {
        *busy = FALSE;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Enables or disables writes to the flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @warning Must be enabled before any writes are performed
 * or any erase is performed.
 *
 * @param enable: 1 if write enable, 0 if write disable
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_write_enable(bool enable) {
    uint16_t tx_buffer[1];
    tx_buffer[0] = (enable ? WRITE_ENABLE : WRITE_DISABLE);

    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        err = mibspi_transmit(flash_1byte_tg, tx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read a register in the flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg: Register to read
 * @param reg_data: Buffer to store returned data
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_read_register(uint8_t reg, uint8_t* reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {(uint16_t)reg, 0xFF};
    uint16_t rx_buffer[2] = {0};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Transmit the read status register command, and receive data
        err = mibspi_transmit_receive(flash_2bytes_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    // Set received data
    *reg_data = (uint8_t)rx_buffer[1];

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Write a register in the flash.
 *
 * @pre @ref mibspi_init from @ref orcasat/interfaces/obc_mibspi.h has been called.
 *
 * @param reg: Register to write
 * @param reg_data: Buffer with data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_write_register(uint8_t reg, const uint8_t* reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {(uint16_t)reg, (uint16_t)reg_data[0]};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        // Transmit the write register command and data to write
        err = mibspi_transmit(flash_2bytes_tg, tx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return FLASH_MIBSPI_MUTEX_GRAB_ERR;
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

#endif
