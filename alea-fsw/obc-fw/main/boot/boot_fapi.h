/**
 * @file boot_fapi.c
 * @brief Wrapper around F021 Flash API
*/

#ifndef BOOT_FAPI_H_
#define BOOT_FAPI_H_

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

boot_err_t boot_fapi_init(uint8_t slot_num);
void boot_fapi_deinit(void);
boot_err_t boot_fapi_erase_slot(uint8_t slot_num);
boot_err_t boot_fapi_write(uint32_t *start_addr, uint8_t *data_buf, uint32_t num_bytes);

#endif // BOOT_FAPI_H_
