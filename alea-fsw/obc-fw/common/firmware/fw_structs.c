/**
 * @file fw_structs.c
 * @brief Handling of data structures stored at the beginning of a firmware image
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "fw_structs.h"

// Firmware
#include "fw_defines.h"

// Platform
#include "fw_memmap.h"

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const volatile fw_info_t FW_INFO __attribute__ ((section (".fwinfo"), retain)) = {
    .magic     = FW_INFO_MAGIC,
    .version   = {FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH, FW_VERSION_TWEAK},
    .githash   = FW_GITHASH,
    .platform  = FW_PLATFORM,
    .target    = FW_TARGET,
    .flags     = FW_FLAGS,
};

const volatile fw_structs_t* FW_STRUCTS[FLASH_SLOT_COUNT] = {
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH0_START_ADDR,
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH1_START_ADDR,
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH2_START_ADDR,
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH3_START_ADDR,
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH4_START_ADDR,
    (const volatile fw_structs_t *)FW_MEMMAP_FLASH5_START_ADDR,
};
