/**
 * @file fw_memmap.h
 * @brief Firmware Memory Map for all platforms
 */

#ifndef FW_MEMMAP_H_
#define FW_MEMMAP_H_

/******************************************************************************/

#define FW_HEADER_MAGIC     0xA1EA50BC // "ALEA'S OBC"
#define FW_ENTRYPOINT_MAGIC 0x10ADC0DE // "LOAD CODE"
#define FW_INFO_MAGIC       0xC001FACE // "COOL FACE"

/******************************************************************************/

#define FW_MEMMAP_EXCFLASH_SIZE   0x00000040
#define FW_MEMMAP_EXCRAM_SIZE     0x00000040
#define FW_MEMMAP_HEADER_SIZE     0x00000010
#define FW_MEMMAP_ENTRYPOINT_SIZE 0x00000010
#define FW_MEMMAP_INFO_SIZE       0x00000010

/******************************************************************************/

#define FLASH_SLOT_STARTUP    0
#define FLASH_SLOT_BOOT_A     1
#define FLASH_SLOT_BOOT_B     2
#define FLASH_SLOT_APP_CORE   3
#define FLASH_SLOT_APP_EXT_A  4
#define FLASH_SLOT_APP_EXT_B  5

#define FLASH_SLOT_COUNT      6

/******************************************************************************/

// STARTUP
#define FW_MEMMAP_FLASH0_START_ADDR (0x00000000 + FW_MEMMAP_EXCFLASH_SIZE)
#define FW_MEMMAP_FLASH0_SIZE       (0x00004000 - FW_MEMMAP_EXCFLASH_SIZE) /* 16 KB - 64 Bytes */

// BOOT A
#define FW_MEMMAP_FLASH1_START_ADDR 0x00004000
#define FW_MEMMAP_FLASH1_SIZE       0x00008000 /* 32 KB */

// BOOT B
#define FW_MEMMAP_FLASH2_START_ADDR 0x0000C000
#define FW_MEMMAP_FLASH2_SIZE       0x00008000 /* 32 KB */

// CORE
#define FW_MEMMAP_FLASH3_START_ADDR 0x00018000
#define FW_MEMMAP_FLASH3_SIZE       0x00028000 /* 160 KB */

// EXT A
#define FW_MEMMAP_FLASH4_START_ADDR 0x00040000
#define FW_MEMMAP_FLASH4_SIZE       0x00040000 /* 256 KB */

// EXT B
#define FW_MEMMAP_FLASH5_START_ADDR 0x00080000
#define FW_MEMMAP_FLASH5_SIZE       0x00040000 /* 256 KB */

/******************************************************************************/

#if (CFG_FLASH_SLOT == FLASH_SLOT_STARTUP)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH0_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH0_SIZE

#elif (CFG_FLASH_SLOT == FLASH_SLOT_BOOT_A)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH1_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH1_SIZE

#elif (CFG_FLASH_SLOT == FLASH_SLOT_BOOT_B)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH2_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH2_SIZE

#elif (CFG_FLASH_SLOT == FLASH_SLOT_APP_CORE)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH3_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH3_SIZE

#elif (CFG_FLASH_SLOT == FLASH_SLOT_APP_EXT_A)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH4_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH4_SIZE

#elif (CFG_FLASH_SLOT == FLASH_SLOT_APP_EXT_B)

    #define FW_MEMMAP_FLASH_START_ADDR  FW_MEMMAP_FLASH5_START_ADDR
    #define FW_MEMMAP_FLASH_SIZE        FW_MEMMAP_FLASH5_SIZE

#endif

/******************************************************************************/

#endif // FW_MEMMAP_H_
