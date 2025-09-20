/**
 * @file boot_updater.c
 * @brief Logic for updating firmware slots
*/

#ifndef BOOT_UPDATER_H_
#define BOOT_UPDATER_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Boot
#include "boot_error.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

boot_err_t boot_preupdate_checks(uint8_t slot_num);
boot_err_t boot_update(uint8_t slot_num);
boot_err_t boot_postupdate_checks(uint8_t slot_num);

#endif // BOOT_UPDATER_H_
