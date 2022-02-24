/**
 * @file comms_defs.c
 * @brief Comms common definitions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

// TODO: change to final values
hwid_t comms_hwid = 0x0109;
hwid_t obc_hwid = 0x7000;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Set comms_hwid
 *
 * @param[in] hwid
 */
void set_comms_hwid(hwid_t hwid) {
    comms_hwid = hwid;
}

/**
 * @brief Set obc_hwid
 *
 * @param[in] hwid
 */
void set_obc_hwid(hwid_t hwid) {
    obc_hwid = hwid;
}
