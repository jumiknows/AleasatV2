/*
 * @file obc_mram.c
 * @brief Driver for the MR25H256ACDF MRAM device
 * @author Steven Richter
 * @date March 5, 2020
 *
 * Datasheet
 * https://www.digikey.ca/product-detail/en/everspin-technologies-inc/MR25H256ACDF/819-1064-ND/8286370
 */

#include "mr25h256.h"
#include "tms_mibspi.h"
#include "gio.h"
#include "reg_het.h"
#include "obc_hardwaredefs.h"
#include "rtos.h"
#include "obc_error.h"

#if FEATURE_MRAM

// MRAM Commands
#define MRAM_WREN  0x06
#define MRAM_WRDI  0x04
#define MRAM_RDSR  0x05
#define MRAM_WRSR  0x01
#define MRAM_READ  0x03
#define MRAM_WRITE 0x02
#define MRAM_SLEEP 0xB9
#define MRAM_WAKE  0xAB

/**
 * @brief Transfer groups.
 */
mibspi_tg_t mram_1byte_tg  = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_1_BYTE_GROUP, .cs_port = MRAM_CS1_PORT, .cs_pin = MRAM_CS1_PIN};
mibspi_tg_t mram_2bytes_tg = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_2_BYTE_GROUP, .cs_port = MRAM_CS1_PORT, .cs_pin = MRAM_CS1_PIN};
mibspi_tg_t mram_4bytes_tg = {.reg = MRAM_MIBSPI_REG, .transfer_group = MRAM_4_BYTE_GROUP, .cs_port = MRAM_CS1_PORT, .cs_pin = MRAM_CS1_PIN};

// Private Function Prototypes
static mram_err_t mram_mr25h256_write_enable(bool enable);

/**
 * @brief Currently does nothing, but exists for API compatibility.
 * @return: MRAM_OK
 */
mram_err_t mram_mr25h256_init(void) {
    return MRAM_OK;
}

/**
 * @brief Program the MRAM with data to store.
 *
 * @pre @ref mram_init from @ref orcasat/hardware-drivers/obc_mram.h has been called.
 *
 * @param addr:         Address to write to, must be byte-aligned
 * @param size_bytes:   Amount of data to write in bytes
 * @param data:         Buffer storing the data to write
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_write(uint16_t addr, uint16_t size_bytes, const uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;
    if ((addr + size_bytes) > MR25H256_MRAM_MAX_ADDRESS) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    // set write enable
    if (mram_mr25h256_write_enable(TRUE) != MRAM_OK) {
        mram_mr25h256_write_enable(FALSE);
        return MRAM_MIBSPI_ERR;
    }
    uint16_t index = 0;
    while (index < size_bytes) {
        /* Transmit write command is structured as follows:
         *    command byte | 16-bit address | 1 byte data buffer
         * Future optimization possible with different transfer group size, since we do one data
         * byte at a time
         */
        uint16_t tx_buffer[4] = {MRAM_WRITE, (addr >> 8) & 0xFF, addr & 0xFF, data[index]};
        // Write to the mram. 4 byte tg <cmd> <addr high> <addr low> <data byte>
        err = mibspi_transmit(mram_4bytes_tg, tx_buffer);
        if (err != MIBSPI_NO_ERR) {
            break;
        }
        // Increment to next data byte write address and index until all indicated bytes
        // transmitted
        addr++;
        index++;
    }
    mram_mr25h256_write_enable(FALSE);
    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief Read data from the mram.
 *
 * @pre @ref mram_init from @ref orcasat/hardware-drivers/obc_mram.h has been called.
 * @details If an error is returned, it is not guaranteed that all data in the read buffer is valid
 *
 * @param addr:         Address to read from, must be byte-aligned
 * @param size_bytes:   Amount of data to read in bytes
 * @param data:         Buffer where read data will be stored
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_read(uint16_t addr, uint16_t size_bytes, uint8_t* data) {
    mibspi_err_t err = MIBSPI_NO_ERR;
    if ((addr + size_bytes) > MR25H256_MRAM_MAX_ADDRESS) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }
    uint16_t index = 0;
    while (index < size_bytes) {
        /* Transmit read command is structured as follows:
         *    command byte | 16-bit address
         */
        uint16_t tx_buffer[4] = {MRAM_READ, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF}; // last val is dummy due to mibspi tx rx
        uint16_t rx_buffer[4] = {0};
        // Read the address in mram
        err = mibspi_transmit_receive(mram_4bytes_tg, tx_buffer, rx_buffer);
        // is this check necessary?
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
 * @pre @ref mram_init from @ref orcasat/hardware-drivers/obc_mram.h has been called.
 *
 * @param reg_data: Buffer to store returned data
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_read_status(uint8_t* reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {MRAM_RDSR, 0xFF}; // Dummy transmit value
    uint16_t rx_buffer[2] = {0};

    // Transmit the read status register command, and receive data
    err = mibspi_transmit_receive(mram_2bytes_tg, tx_buffer, rx_buffer);

    // Set received data
    *reg_data = (uint8_t)rx_buffer[1];

    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief Write the status register of the mram
 *
 * @pre @ref mram_init from @ref orcasat/hardware-drivers/obc_mram.h has been called.
 *
 * @param reg_data: Buffer with data to write
 * @return: MRAM_OK if no error, error code otherwise
 */
mram_err_t mram_mr25h256_write_status(const uint8_t reg_data) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[2] = {MRAM_WRSR, reg_data};

    // set write enable
    if (mram_mr25h256_write_enable(TRUE) != MRAM_OK) {
        mram_mr25h256_write_enable(FALSE);
        return MRAM_MIBSPI_ERR;
    }
    // Transmit the write status register command and data to write
    err = mibspi_transmit(mram_2bytes_tg, tx_buffer);

    mram_mr25h256_write_enable(FALSE);

    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

/**
 * @brief toggles reset of MRAM by cutting power
 * @pre @ref het_init from @ref platform/include/het.h has been called.
 */
void mram_mr25h256_reset(void) {
    gioSetBit(MRAM_RST_PORT, MRAM_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gioSetBit(MRAM_RST_PORT, MRAM_RST_PIN, 0);
}

/* Private Functions */

/**
 * @brief Enables or disables writes to MRAM
 *
 * @pre @ref mram_init from @ref orcasat/hardware-drivers/obc_mram.h has been called.
 *
 * @warning Must be enabled before any writes are performed
 *
 *
 * @param enable: 1 if write enable, 0 if write disable
 * @return: MRAM_OK if no error, error code otherwise
 */
static mram_err_t mram_mr25h256_write_enable(bool enable) {
    uint16_t tx_buffer[1];
    tx_buffer[0]     = (enable ? MRAM_WREN : MRAM_WRDI);
    mibspi_err_t err = MIBSPI_NO_ERR;

    err = mibspi_transmit(mram_1byte_tg, tx_buffer);
    return (err != MIBSPI_NO_ERR) ? MRAM_MIBSPI_ERR : MRAM_OK;
}

#endif // FEATURE_MRAM
