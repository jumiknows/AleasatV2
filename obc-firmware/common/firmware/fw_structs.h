/**
 * @file fw_structs.h
 * @brief Handling of data structures stored at the beginning of a firmware image
 */

#ifndef FW_STRUCTS_H_
#define FW_STRUCTS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Platform
#include "fw_memmap.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    uint32_t burn_number;
    uint32_t magic;
    uint32_t size;
    uint32_t crc32;
} fw_header_t;

typedef struct {
    uint32_t magic;
    uint32_t flash_addr;
    uint32_t jump_instr;
    uint32_t jump_instr2;
} fw_entrypoint_t;

typedef struct {
    uint32_t magic;
    uint8_t version[4];
    uint32_t githash;
    uint8_t platform;
    uint8_t target;
    uint16_t flags;
} fw_info_t;

typedef struct {
    fw_header_t header;
    fw_entrypoint_t entrypoint;
    fw_info_t info;
} fw_structs;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

/**
 * @brief Linker-defined symbol for this firmware image's header
 */
extern const volatile fw_header_t FW_HEADER;

/**
 * @brief Linker-defined symbol for this firmware image's entrypoint
 */
extern const volatile fw_entrypoint_t FW_ENTRYPOINT;

/**
 * @brief This firmware image's info (defined in fw_structs.c)
 */
extern const volatile fw_info_t FW_INFO;

/**
 * @brief Array of pointers to the firmware structs for all firmware images
 *        that may be present on the device
 */
extern const volatile fw_structs* FW_STRUCTS[FLASH_SLOT_COUNT];

#endif // FW_STRUCTS_H_
