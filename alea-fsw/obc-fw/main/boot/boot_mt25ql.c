/**
 * @file boot_mt25ql.c
 * @brief Minimal driver for the MT25QL256 NOR Flash device for use in bootloader
 *
 * Datasheet: https://www.micron.com/products/nor-flash/serial-nor-flash/part-catalog/mt25ql256aba1ew9-0sit
*/

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "boot_mt25ql.h"

// Boot
#include "boot_mibspi.h"
#include "boot_error.h"

// OBC
#include "obc_flashdefs.h"

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

#define MANUFACTURER_ID_JEDEC_MICRON_TECHNOLOGY  0x20

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the flash chip.
 */
static mibspi_tg_t flash_1byte_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_1_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_20bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_20_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

static mibspi_tg_t flash_69bytes_tg = {.reg = FLASH_MIBSPI_REG, .transfer_group = FLASH_69_BYTE_GROUP, .cs_port = FLASH_CS0_PORT, .cs_pin = FLASH_CS0_PIN};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static boot_err_t boot_mt25ql_enable_4byte_addressing(void);
static boot_err_t boot_mt25ql_read_id(void);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

boot_err_t boot_mt25ql_init(void) {
    boot_err_t err = boot_mt25ql_enable_4byte_addressing();

    if (err != BOOT_SUCCESS) {
        return err;
    }

    return boot_mt25ql_read_id();
}

/**
 * @brief Read a 64 byte chunk from the hash (1/4 of a 256 byte page)
 *
 * @param addr: Address to read from, must be 64 byte aligned
 * @param data: Buffer where read data will be stored
 *
 * @return boot_err_t
 */
boot_err_t boot_mt25ql_read_64(uint32_t addr, uint8_t *data) {

    // Validate no data will be read outside address bounds
    if ((addr + READ_WRITE_DATA_LEN) > FLASH_ADDR_MAX + 1) {
        return BOOT_EXT_FLASH_ERR;
    }

    // Validate address alignment
    if ((addr % READ_WRITE_DATA_LEN) != 0) {
        return BOOT_EXT_FLASH_ERR;
    }

    /* Transmit read command is structured as follows:
     *    command byte | 32-bit address | N-byte meaningless data (to clock in read bytes)
     */
    uint16_t tx_buffer[READ_WRITE_DATA_LEN + 5] = {FLASH_READ_4B_ADDR, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, [5 ...(READ_WRITE_DATA_LEN + 4)] = 0xFF};
    uint16_t rx_buffer[READ_WRITE_DATA_LEN + 5] = {0};

    boot_err_t err = boot_mibspi_tx_rx(&flash_69bytes_tg, tx_buffer, rx_buffer);

    if (err != BOOT_SUCCESS) {
        return BOOT_MIBSPI_ERR;
    }

    // Copy the read data into the receive buffer
    for (uint32_t i = 0; i < READ_WRITE_DATA_LEN; i++) {
        data[i] = rx_buffer[5 + i];
    }

    return BOOT_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Enable 4 byte addressing mode
 */
static boot_err_t boot_mt25ql_enable_4byte_addressing(void) {
    uint16_t tx_buffer[1] = {FLASH_ENTER_4BYTE_ADDR_MODE};

    return boot_mibspi_tx(&flash_1byte_tg, tx_buffer);
}

/**
 * @brief Reads the Device ID data from the MT25QL
 *
 * @param device_data: buffer to store raw device data (must be > 20 bytes)
 *
 * @return boot_err_t
 */
static boot_err_t boot_mt25ql_read_id(void) {
    uint16_t tx_buffer[20] = {FLASH_RDID, [1 ... 19] = 0xFF};
    uint16_t rx_buffer[20] = { 0x00 };

    boot_err_t err = boot_mibspi_tx_rx(&flash_20bytes_tg, tx_buffer, rx_buffer);

    if (err != BOOT_SUCCESS) {
        return BOOT_MIBSPI_ERR;
    }

    // Ignoring the first byte (dummy data)
    if (rx_buffer[1] == MANUFACTURER_ID_JEDEC_MICRON_TECHNOLOGY) {
        return BOOT_SUCCESS;
    }

    return BOOT_EXT_FLASH_ERR;
}
