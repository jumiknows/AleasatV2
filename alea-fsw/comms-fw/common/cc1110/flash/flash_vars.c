/**
 * @file flash_vars.c
 * @brief Variables to access data stored in flash. Mirrors flash_reserve.c in bootloader
 */

#include <stdint.h>
#include "flash_vars.h"
#include "hwid.h"
#include "flash_constants.h"

__code __at (FLASH_SIGNATURE_KEYS) uint8_t  flash_keys[FLASH_RESERVED - FLASH_SIGNATURE_KEYS];
__code __at (FLASH_RESERVED)       uint16_t reserved_flash;
__code __at (FLASH_HWID)           hwid_t   hwid_flash;
