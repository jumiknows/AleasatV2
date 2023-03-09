/**
 * @file flash_mt25ql.c
 * @brief Driver for the MT25QL256 NOR Flash device.
 *
 * Datasheet: https://www.micron.com/products/nor-flash/serial-nor-flash/part-catalog/mt25ql256aba1ew9-0sit
*/

#ifdef PLATFORM_ALEA_V1

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "flash_mt25ql.h"

// OBC
#include "tms_mibspi.h"
#include "alea_v1_hardwaredefs.h"

// HAL
#include "gio.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief The highest address possible for erase, write, and read operations
 */
#define FLASH_ADDR_MAX 0x01FFFFFF

/**
 * @brief Number of bytes of flash data per read or write transaction
 */
#define READ_WRITE_DATA_LEN 64

/**
 * @brief The size in bytes of the maximum programmable page
 */
#define FLASH_PAGE_SIZE_BYTES 0x100

/**
 * @brief MIBSPI timeout for MT25QL flash driver
 */
#define FLASH_MIBSPI_TIMEOUT_MS 30

/**
 * @brief Maximum time an erase operation could take. See max bulk erase
 * time in datasheet on page 90.
 */
#define MAX_ERASE_TIMEOUT_MS (231 * 1000)

/**
 * @brief The time to allow for other tasks to run between checks for erase
 * completion.
 */
#define FLASH_ERASE_INCREMENT_MS 10

/**
 * @brief The time to allow for other tasks to run between checks for write
 * completion.
 */
#define FLASH_WRITE_INCREMENT_MS 10

/**
 * @brief Maximum time a write operation could take for 256 bytes. See max
 * page program time in datasheet on page 90.
 */
#define MAX_WRITE_TIMEOUT_MS 2

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the flash chip.
 */
static mibspi_tg_t flash_1byte_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_1_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_2bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_2_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_5bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_5_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_20bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_20_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_69bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_69_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static flash_err_t mt25ql_is_busy(bool* busy);
static flash_err_t mt25ql_write_enable(bool enable);
static flash_err_t mt25ql_read_register(uint8_t reg, uint8_t* reg_data);
static flash_err_t mt25ql_write_register(uint8_t reg, const uint8_t* reg_data);
static flash_err_t mt25ql_enable_4byte_addressing(bool enable);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the MT25QL driver
 */
void mt25ql_init(void) {
    /*
     * Enable MIBSPI group notifications for all TGs
     */
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_1_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_2_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_5_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_20_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_69_BYTE_GROUP, 0U);

    /*
     * Enable 4 byte addressing mode (this is currently only needed for 32KB sector erases)
     */
    mt25ql_enable_4byte_addressing(true);
}

/**
 * @brief Reads the Device ID data from the MT25QL
 * 
 * @param device_data: buffer to store raw device data (must be > 20 bytes)
 * 
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read_id(uint8_t* device_data) {
    mibspi_err_t err = MIBSPI_NO_ERR;
    uint16_t tx_buffer[20] = {FLASH_RDID, [1 ... 19] = 0xFF};
    uint16_t rx_buffer[20] = { 0x00 };

    err = tms_mibspi_tx_rx(&flash_20bytes_tg, tx_buffer, rx_buffer, FLASH_MIBSPI_TIMEOUT_MS);

    // Copy data from rx buffer to device data buf, ignoring the first byte (dummy data)
    for (uint8_t i = 0; i < (sizeof(rx_buffer) / sizeof(uint16_t) - 1); i++) {
        device_data[i] = (uint8_t) rx_buffer[i + 1];
    }
    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Reads the status register.
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
 * @param reg_data: buffer to store the read register value
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read_flag_status_register(uint8_t* reg_data) {
    return mt25ql_read_register(READ_FLAG_STATUS_REG, reg_data);
}

/**
 * @brief Writes to the status register.
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
 * @param addr: Address within the sector/block that is to be erased, must be byte-aligned.
 * @param erase_size: Block size to erase. This flash can do 64KB, 32KB, 4KB, or full flash.
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_erase(uint32_t addr, flash_erase_sz_t erase_size) {
    /* Note: there is no 4-byte version of sector_erase_32kb. However, we enable 4 byte addressing mode during init */
    static const uint8_t erase_cmds[] = {BULK_ERASE, SECTOR_ERASE_4B_ADDR, SECTOR_ERASE_32KB, SECTOR_ERASE_4KB_4B_ADDR};

    mibspi_err_t err = MIBSPI_NO_ERR;

    // Validate address
    if (addr > FLASH_ADDR_MAX) {
        return FLASH_ADDR_OR_SIZE_TOO_LARGE_ERR;
    }

    // Set write enable
    if (mt25ql_write_enable(TRUE) != FLASH_OK) {
        return FLASH_MIBSPI_ERR;
    }

    uint16_t tx_buffer[5] = {(uint16_t)erase_cmds[erase_size], (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

    // Erase
    if (erase_size == FULL_CHIP) {
        err = tms_mibspi_tx(&flash_1byte_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
    } else {
        err = tms_mibspi_tx(&flash_5bytes_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
    }
    if (err != MIBSPI_NO_ERR) {
        return FLASH_MIBSPI_ERR;
    }

    // Poll until the erase has been completed
    bool busy              = TRUE;
    flash_err_t err_flash  = FLASH_OK;
    uint8_t timeout_ctr_ms = 0;
    do {
        if (timeout_ctr_ms == MAX_ERASE_TIMEOUT_MS) {
            return FLASH_ERASE_TIMEOUT_ERR;
        }
        timeout_ctr_ms += FLASH_ERASE_INCREMENT_MS;
        vTaskDelay(pdMS_TO_TICKS(FLASH_ERASE_INCREMENT_MS));
        err_flash = mt25ql_is_busy(&busy);
    } while ((busy == TRUE) && (err_flash == FLASH_OK));

    return err_flash;
}

/**
 * @brief Program the flash with a 64 byte chunk of data (1/4 of a 256 byte page)
 *
 * @param addr: Address to write to, must be 64 byte aligned
 * @param data: Buffer storing the data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_write_64(uint32_t addr, const uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    // Validate no data will be written outside address bounds
    if ((addr + READ_WRITE_DATA_LEN) > FLASH_ADDR_MAX + 1) {
        return FLASH_ADDR_OR_SIZE_TOO_LARGE_ERR;
    }

    // Validate address alignment
    if ((addr % READ_WRITE_DATA_LEN) != 0) {
        return FLASH_ADDR_UNALIGNED_ERR;
    }

    // Set write enable
    if (mt25ql_write_enable(TRUE) != FLASH_OK) {
        return FLASH_MIBSPI_ERR;
    }

    /* Transmit write command is structured as follows:
     *    command byte | 32-bit address | N-byte data buffer
     */
    uint16_t tx_buffer[READ_WRITE_DATA_LEN + 5] = {FLASH_WRITE_4B_ADDR, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, [5 ... READ_WRITE_DATA_LEN + 4] = 0xFF};

    for (uint32_t i = 0; i < READ_WRITE_DATA_LEN; i++) {
        tx_buffer[5 + i] = data[i];
    }

    err = tms_mibspi_tx(&flash_69bytes_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
    if (err != MIBSPI_NO_ERR) {
        return FLASH_MIBSPI_ERR;
    }

    // Poll until the write has been completed
    bool busy              = TRUE;
    flash_err_t err_flash  = FLASH_OK;
    uint8_t timeout_ctr_ms = 0;
    do {
        if (timeout_ctr_ms == MAX_WRITE_TIMEOUT_MS) {
            return FLASH_WRITE_TIMEOUT_ERR;
        }
        timeout_ctr_ms += FLASH_WRITE_INCREMENT_MS;
        vTaskDelay(pdMS_TO_TICKS(FLASH_WRITE_INCREMENT_MS));
        err_flash = mt25ql_is_busy(&busy);
    } while ((busy == TRUE) && (err_flash == FLASH_OK));

    return err_flash;
}

/**
 * @brief Read a 64 byte chunk from the hash (1/4 of a 256 byte page)
 *
 * @param addr: Address to read from, must be 64 byte aligned
 * @param data: Buffer where read data will be stored
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t mt25ql_read_64(uint32_t addr, uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    // Validate no data will be read outside address bounds
    if ((addr + READ_WRITE_DATA_LEN) > FLASH_ADDR_MAX + 1) {
        return FLASH_ADDR_OR_SIZE_TOO_LARGE_ERR;
    }

    // Validate address alignment
    if ((addr % READ_WRITE_DATA_LEN) != 0) {
        return FLASH_ADDR_UNALIGNED_ERR;
    }

    /* Transmit read command is structured as follows:
     *    command byte | 32-bit address | N-byte meaningless data (to clock in read bytes)
     */
    uint16_t tx_buffer[READ_WRITE_DATA_LEN + 5] = {FLASH_READ_4B_ADDR, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, [5 ... (READ_WRITE_DATA_LEN+4)] = 0xFF};
    uint16_t rx_buffer[READ_WRITE_DATA_LEN + 5] = {0};

    err = tms_mibspi_tx_rx(&flash_69bytes_tg, tx_buffer, rx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
    if (err != MIBSPI_NO_ERR) {
        return FLASH_MIBSPI_ERR;
    }

    // Copy the read data into the receive buffer
    for (uint32_t i = 0; i < READ_WRITE_DATA_LEN; i++) {
        data[i] = rx_buffer[5 + i];
    }

    return FLASH_OK;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Checks if a write or erase is currently in progress.
 *
 * @param busy: Stores the status of the being busy
 * @return FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_is_busy(bool* busy) {
    mibspi_err_t err;

    do {
        uint16_t tx_buffer[2] = {READ_FLAG_STATUS_REG, 0xFF};
        uint16_t rx_buffer[2] = {0};

        err = tms_mibspi_tx_rx(&flash_2bytes_tg, tx_buffer, rx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
        if (err != MIBSPI_NO_ERR) {
            break;
        }

        if (((uint8_t)rx_buffer[1] & 0x80) == 0) {
            *busy = TRUE;
        } else {
            *busy = FALSE;
        }
    } while (0);

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Enables or disables writes to the flash.
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

    mibspi_err_t err = tms_mibspi_tx(&flash_1byte_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Read a register in the flash.
 *
 * @param reg: Register to read
 * @param reg_data: Buffer to store returned data
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_read_register(uint8_t reg, uint8_t* reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {(uint16_t)reg, 0xFF};
    uint16_t rx_buffer[2] = {0};

    // Transmit the read status register command, and receive data
    err = tms_mibspi_tx_rx(&flash_2bytes_tg, tx_buffer, rx_buffer, FLASH_MIBSPI_TIMEOUT_MS);

    if (err == MIBSPI_NO_ERR) {
        // Set received data
        *reg_data = (uint8_t)rx_buffer[1];
    }

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Write a register in the flash.
 *
 * @param reg: Register to write
 * @param reg_data: Buffer with data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_write_register(uint8_t reg, const uint8_t* reg_data) {
    uint16_t tx_buffer[2] = {(uint16_t)reg, (uint16_t)reg_data[0]};

    // Transmit the write register command and data to write
    mibspi_err_t err = tms_mibspi_tx(&flash_2bytes_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);

    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

/**
 * @brief Enable/disable 4 byte addressing mode
 * 
 * @param enable: true to enable 4 byte addressing mode, false to disable
 * @return: FLASH_OK if no error, error code otherwise
 */
static flash_err_t mt25ql_enable_4byte_addressing(bool enable) {
    uint16_t tx_buffer[1] = {enable ? FLASH_ENTER_4BYTE_ADDR_MODE : FLASH_EXIT_4BYTE_ADDR_MODE};

    mibspi_err_t err = tms_mibspi_tx(&flash_1byte_tg, tx_buffer, FLASH_MIBSPI_TIMEOUT_MS);
    return (err != MIBSPI_NO_ERR) ? FLASH_MIBSPI_ERR : FLASH_OK;
}

#endif // PLATFORM_ALEA_V1
