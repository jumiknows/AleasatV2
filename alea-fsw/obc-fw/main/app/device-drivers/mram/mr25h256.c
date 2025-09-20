/*
 * @file obc_mram.c
 * @brief Driver for the MR25H256ACDF MRAM device
 *
 * Datasheet
 * https://www.digikey.ca/product-detail/en/everspin-technologies-inc/MR25H256ACDF/819-1064-ND/8286370
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "mr25h256.h"
#include "tms_mibspi.h"
#include "gio.h"
// #include "obc_hardwaredefs.h"
#include "launchpad_1224_hardwaredefs.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/
static mibspi_err_t mram_mr25h256_write_enable(bool enable);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/
// MRAM Commands
#define MRAM_WREN  0x06
#define MRAM_WRDI  0x04
#define MRAM_RDSR  0x05
#define MRAM_WRSR  0x01
#define MRAM_READ  0x03
#define MRAM_WRITE 0x02

/**
 * @brief Transfer groups.
 */
static mibspi_tg_t mram_1byte_tg   = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_1_BYTE_GROUP, .cs_port = MRAM_CS_PORT, .cs_pin = MRAM_CS_PIN};
static mibspi_tg_t mram_2bytes_tg  = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_2_BYTE_GROUP, .cs_port = MRAM_CS_PORT, .cs_pin = MRAM_CS_PIN};
static mibspi_tg_t mram_4bytes_tg  = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_4_BYTE_GROUP, .cs_port = MRAM_CS_PORT, .cs_pin = MRAM_CS_PIN};
static mibspi_tg_t mram_11bytes_tg = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_11_BYTE_GROUP, .cs_port = MRAM_CS_PORT, .cs_pin = MRAM_CS_PIN};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the MR25H256 driver
 * @return: MRAM_OK
 */
mram_err_t mram_mr25h256_init(void) {
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_1_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_2_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_4_BYTE_GROUP, 0U);
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_11_BYTE_GROUP, 0U);

    return MRAM_OK;
}

/**
 * @brief Program the MRAM with data to store.
 *
 * @pre @ref mram_init from @ref device-drivers/obc_mram.h has been called.
 *
 * @param addr:         Address to write to, must be byte-aligned
 * @param size_bytes:   Amount of data to write in bytes
 * @param data:         Buffer storing the data to write
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data) {
    // The address will roll over to 0000h when the address reaches top of memory
    // add max checking so writes aren't confusing
    if ((addr + size_bytes) > MR25H256_MRAM_MAX_ADDRESS) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    // set write enable
    mibspi_err_t err = mram_mr25h256_write_enable(true);

    if (err != MIBSPI_NO_ERR) {
        mram_mr25h256_write_enable(false);
        return MRAM_MIBSPI_ERR;
    }

    uint16_t index = 0;

    while (size_bytes > 0) {
        /* Transmit write command is structured as follows:
         *    command byte | 16-bit address | byte data buffer
         *
         * Transfer 8 bytes at a time, until there are less than 8 bytes, then switch to 1 byte
         */
        uint8_t write_size = 8;

        if (size_bytes >= 8) {
            uint16_t tx_buffer[11];
            tx_buffer[0] = MRAM_WRITE;
            tx_buffer[1] = (addr >> 8) & 0xFF;
            tx_buffer[2] = (addr & 0xFF);

            for (uint8_t i = 0; i < 8; i++) {
                tx_buffer[i + 3] = data[index + i];
            }

            err = tms_mibspi_tx(&mram_11bytes_tg, tx_buffer, MRAM_MIBSPI_TIMEOUT_MS);
        } else {
            uint16_t tx_buffer[4] = {MRAM_WRITE, (addr >> 8) & 0xFF, addr & 0xFF, data[index]};
            err                   = tms_mibspi_tx(&mram_4bytes_tg, tx_buffer, MRAM_MIBSPI_TIMEOUT_MS);
            write_size            = 1;
        }

        // stop if something went wrong
        if (err != MIBSPI_NO_ERR) {
            break;
        }

        addr += write_size;
        index += write_size;
        size_bytes -= write_size;
    }

    mram_mr25h256_write_enable(false);
    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief Read data from the mram.
 *
 * @pre @ref mram_init from @ref device-drivers/obc_mram.h has been called.
 * @details If an error is returned, it is not guaranteed that all data in the read buffer is valid
 *
 * @param addr:         Address to read from, must be byte-aligned
 * @param size_bytes:   Amount of data to read in bytes
 * @param data:         Buffer where read data will be stored
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_read(uint16_t addr, uint16_t size_bytes, uint8_t *data) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    if ((addr + size_bytes) > MR25H256_MRAM_MAX_ADDRESS) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    uint16_t index = 0;

    while (index < size_bytes) {
        /* Transmit read command is structured as follows:
         *    command byte | 16-bit address
         *
         * Note: reads continue as long as memory is clocked
         */
        uint16_t tx_buffer[4] = {MRAM_READ, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF}; // last val is dummy due to mibspi tx rx
        uint16_t rx_buffer[4] = {0};
        // Read the address in mram
        err = tms_mibspi_tx_rx(&mram_4bytes_tg, tx_buffer, rx_buffer, MRAM_MIBSPI_TIMEOUT_MS);

        if (err != MIBSPI_NO_ERR) {
            break;
        }

        // copy rx buffer into our data buffer
        data[index] = (uint8_t)rx_buffer[3];

        // Increment to next read address and index
        addr++;
        index++;
    }

    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief Read the status register of the mram
 *
 * @param reg_data: Buffer to store returned data
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_read_status(uint8_t *reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {MRAM_RDSR, 0xFF}; // Dummy transmit value
    uint16_t rx_buffer[2] = {0};

    // Transmit the read status register command, and receive data
    err = tms_mibspi_tx_rx(&mram_2bytes_tg, tx_buffer, rx_buffer, MRAM_MIBSPI_TIMEOUT_MS);

    // Set received data
    *reg_data = (uint8_t)rx_buffer[1];

    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief Write the status register of the mram
 *
 * @pre @ref mram_init from @ref device-drivers/obc_mram.h has been called.
 *
 * @param reg_data: Buffer with data to write
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_write_status(const uint8_t reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {MRAM_WRSR, reg_data};

    // set write enable
    if (mram_mr25h256_write_enable(true) != MIBSPI_NO_ERR) {
        mram_mr25h256_write_enable(false);
        return MRAM_MIBSPI_ERR;
    }

    // Transmit the write status register command and data to write
    err = tms_mibspi_tx(&mram_2bytes_tg, tx_buffer, MRAM_MIBSPI_TIMEOUT_MS);

    mram_mr25h256_write_enable(false);

    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Enables or disables writes to MRAM
 *
 * @warning Must be enabled before any writes are performed
 *
 * @param enable: 1 if write enable, 0 if write disable
 * @return: MRAM_OK if no error, error code otherwise
 */
static mibspi_err_t mram_mr25h256_write_enable(bool enable) {
    uint16_t tx_buffer[1] = {enable ? MRAM_WREN : MRAM_WRDI};
    return tms_mibspi_tx(&mram_1byte_tg, tx_buffer, MRAM_MIBSPI_TIMEOUT_MS);
}
