/**
 * @file boot_mibspi.h
 * @brief Minimal mibspi driver wrapper for use in bootloader.
 */

#ifndef BOOT_MIBSPI_H_
#define BOOT_MIBSPI_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Boot
#include "boot_error.h"

// HAL
#include "mibspi.h"
#include "sys_common.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Wrapper for a MIBSPI transfer group.
 *
 * Contains a pointer to the mibspiBASE_t* to be used in this transaction,
 * as well as information about the chip select.
 */
typedef struct {
    mibspiBASE_t *reg;
    uint8_t transfer_group;
    gioPORT_t *cs_port;
    uint32_t cs_pin;
} mibspi_tg_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void boot_mibspi_init(void);
boot_err_t boot_mibspi_tx_rx(const mibspi_tg_t *tg, uint16_t *tx_buffer, uint16_t *rx_buffer);
boot_err_t boot_mibspi_tx(const mibspi_tg_t *tg, uint16_t *tx_buffer);

#endif // BOOT_MIBSPI_H_
