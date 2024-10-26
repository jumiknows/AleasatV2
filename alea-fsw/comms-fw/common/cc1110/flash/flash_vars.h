#ifndef _FLASH_VARS
#define _FLASH_VARS

#include <stdint.h>
#include "hwid.h"
#include "flash_constants.h"

extern __code __at (FLASH_SIGNATURE_KEYS) uint8_t  flash_keys[FLASH_RESERVED - FLASH_SIGNATURE_KEYS];
extern __code __at (FLASH_RESERVED)       uint16_t reserved_flash;
extern __code __at (FLASH_HWID)           hwid_t   hwid_flash;

#endif /* _FLASH_VARS */
