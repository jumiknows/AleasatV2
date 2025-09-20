/**
 * @file boot_mibspi.c
 * @brief Minimal mibspi driver wrapper for use in bootloader.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "boot_mibspi.h"

// Boot
#include "boot_error.h"

// HAL
#include "gio.h"
#include "mibspi.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/* Number of while loop iterations to wait before timeout
 * Usually ~200 loops
 */
#define MIBSPI_TIMEOUT_LOOPS 1000

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void boot_mibspi_init(void) {
    mibspiInit();
}

/**
 * @brief Transmit data and wait for received data using a particular MibSPI transfer group.
 *
 * @pre boot_mibspi_init()
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 *
 * @return boot_err_t
 */
boot_err_t boot_mibspi_tx_rx(const mibspi_tg_t *tg, uint16_t *tx_buffer, uint16_t *rx_buffer) {

    if (boot_mibspi_tx(tg, tx_buffer) != BOOT_SUCCESS) {
        return BOOT_MIBSPI_ERR;
    }

    // Recv data
    uint32_t err = mibspiGetData(tg->reg, tg->transfer_group, rx_buffer);

    return err ? BOOT_MIBSPI_ERR : BOOT_SUCCESS;
}

/**
 * @brief Transmit data  using a particular MibSPI transfer group.
 *
 * @pre boot_mibspi_init()
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 *
 * @return boot_err_t
 */
boot_err_t boot_mibspi_tx(const mibspi_tg_t *tg, uint16_t *tx_buffer) {
    // Set up data to be transferred
    mibspiSetData(tg->reg, tg->transfer_group, tx_buffer);

    // Enable CS
    gioSetBit(tg->cs_port, tg->cs_pin, 0);

    // Transfer Data
    mibspiTransfer(tg->reg, tg->transfer_group);

    // Wait for MIBSPI transfer complete
    uint32_t num_loops = 0;

    while (!mibspiIsTransferComplete(tg->reg, tg->transfer_group)) {
        num_loops++;

        if (num_loops > MIBSPI_TIMEOUT_LOOPS) {
            return BOOT_MIBSPI_ERR;
        }
    }

    // Disable CS
    gioSetBit(tg->cs_port, tg->cs_pin, 1);

    return BOOT_SUCCESS;
}
