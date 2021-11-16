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

// TODO: change to final value
comms_hwid_t comms_hwid = 0x0107;
uint8_t comms_system = 1;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Set comms_hwid
 *
 * @param[in] hwid
 */
void set_comms_hwid(comms_hwid_t hwid) {
    comms_hwid = hwid;
}

/**
 * @brief Set comms_system
 *
 * @param[in] system
 */
void set_comms_system(uint8_t system) {
    comms_system = system;
}
