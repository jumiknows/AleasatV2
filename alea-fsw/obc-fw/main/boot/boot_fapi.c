/**
 * @file boot_fapi.c
 * @brief Wrapper around F021 Flash API
*/

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "boot_fapi.h"

// Boot
#include "boot_error.h"

// OBC
#include "fw_memmap.h"
#include "dabort.h"
#include "asm_utils.h"

// HAL
#include "system.h"
#include "sys_core.h"

// Lib
#include "F021.h" // This comes last since it tries to redefine some macros from stdbool
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/* Number of while loop iterations to wait before timeout
 * Usually takes ~70000 loops for a 128KB erase
 * Benchmarked at 96MHz CPU clock
 */
#define FAPI_TIMEOUT_LOOPS 1000000

/* Suspending the FSM during an erase operation takes around 2.5ms (state machine requirement)
 * https://e2e.ti.com/support/microcontrollers/arm-based-microcontrollers-group/arm-based-microcontrollers/f/arm-based-microcontrollers-forum/541973/suspending-f021-flash-api-operations
 */
#define FSM_DELAY_MS    100U
#define FSM_DELAY_LOOPS (FSM_DELAY_MS * 1000U * ((uint32_t)GCLK_FREQ) / 5U) // 5 is the number of cycles / loop

/**
 * @brief Can only do accesses of 16-bytes long at a time, this is limitation
 * of the flash bank width.
 * Reference manual page 113: flash banks are 144-bit(18-byte) wide with ECC
 *                  page 114: 8-bit ECC for every 64-bits of data
 *
 * So the actual bank width is 144 - 2 * 8 = 128-bits = 16 bytes
 */
#define FLASH_WRITE_SIZE 16

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/* The following symbols are defined at link time (see main/linker.cmd)
 * Locations and size of F021 flash API
 */
extern unsigned int f021_load;
extern unsigned int f021_run;
extern unsigned int f021_size;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static bool boot_fapi_sector_erase(uint32_t sector1_addr, uint32_t sector1_len, uint32_t sector2_addr, uint32_t sector2_len);
static bool boot_fapi_enable_sectors(uint8_t slot_num);
static inline bool boot_fapi_check_erase(uint32_t sector_addr, uint32_t sector_len);
static inline bool boot_fapi_poll_fsm_status(void);
static void boot_fapi_reset_fsm(void);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes flash bank for programming
 *
 * Flash ECC must be disabled as erasing the flash results in invalid ECC conditions
 *
 * @return boot_err_t
 */
boot_err_t boot_fapi_init(uint8_t slot_num) {

    _coreDisableFlashEcc_();

    // Initialize internal flash
    if (Fapi_initializeFlashBanks((uint32_t)HCLK_FREQ) != Fapi_Status_Success) {
        return BOOT_INT_FLASH_INIT_ERR;
    }

    if (Fapi_setActiveFlashBank(Fapi_FlashBank0) != Fapi_Status_Success) {
        return BOOT_INT_FLASH_INIT_ERR;
    }

    return boot_fapi_enable_sectors(slot_num) ? BOOT_SUCCESS : BOOT_INT_FLASH_INIT_ERR;
}

/**
 * @brief Restores Flash ECC after all erase and program commands have finished
 */
void boot_fapi_deinit(void) {

    dabort_FUNC_ERR_set_nonfatal();

    _coreEnableFlashEcc_();

    // Check for flash ECC errors and clear if necessary
    if (dabort_FUNC_ERR_get()) {
        dabort_FUNC_ERR_clear();
    }

    dabort_FUNC_ERR_set_fatal();
}

/**
 * @brief Erases & verifies the specified firmware slot and prepares it for programming
 *
 * @return boot_err_t
 */
boot_err_t boot_fapi_erase_slot(uint8_t slot_num) {
    if (slot_num == 0 || slot_num > 5) {
        return BOOT_INVAL_ERR;
    }

    switch (slot_num) {
    case FLASH_SLOT_BOOT_A:
        return boot_fapi_sector_erase(FW_MEMMAP_FLASH1_START_ADDR, 4096, 0x00008000, 4096) ? BOOT_SUCCESS : BOOT_INT_FLASH_ERASE_ERR;

    case FLASH_SLOT_BOOT_B:
        return boot_fapi_sector_erase(FW_MEMMAP_FLASH2_START_ADDR, 4096, 0x00010000, 4096) ? BOOT_SUCCESS : BOOT_INT_FLASH_ERASE_ERR;

    case FLASH_SLOT_APP_CORE:
        return boot_fapi_sector_erase(FW_MEMMAP_FLASH3_START_ADDR, 8192, 0x00020000, 32768) ? BOOT_SUCCESS : BOOT_INT_FLASH_ERASE_ERR;

    case FLASH_SLOT_APP_EXT_A:
        return boot_fapi_sector_erase(FW_MEMMAP_FLASH4_START_ADDR, 32768, 0x00060000, 32768) ? BOOT_SUCCESS : BOOT_INT_FLASH_ERASE_ERR;

    case FLASH_SLOT_APP_EXT_B:
        return boot_fapi_sector_erase(FW_MEMMAP_FLASH5_START_ADDR, 32768, 0x000A0000, 32768) ? BOOT_SUCCESS : BOOT_INT_FLASH_ERASE_ERR;

    default:
        return BOOT_INVAL_ERR;
    }
}

/**
 * @brief Write to internal flash
 *
 * @param[in/out] start_addr       Pointer to start addr of flash to write to (value incremented by num_bytes after a write)
 * @param[in]     data_buf         Pointer to new fw image
 * @param[in]     num_bytes        Number of bytes to write (must be a multiple of FLASH_WRITE_SIZE)
 *
 * @return boot_err_t
 */
boot_err_t boot_fapi_write(uint32_t *start_addr, uint8_t *data_buf, uint32_t num_bytes) {
    // num_bytes must be a multiple of FLASH_WRITE_SIZE (limited by bank width)
    if (num_bytes % FLASH_WRITE_SIZE != 0) {
        return BOOT_INVAL_ERR;
    }

    for (uint32_t i = 0; i < num_bytes; i += FLASH_WRITE_SIZE) {
        Fapi_issueProgrammingCommand(start_addr, data_buf, FLASH_WRITE_SIZE, 0, 0, Fapi_AutoEccGeneration);

        if (!boot_fapi_poll_fsm_status()) {
            boot_fapi_reset_fsm();
            return BOOT_INT_FLASH_WRITE_ERR;
        }

        Fapi_FlashStatusWordType poFlashStatusWord;

        if (Fapi_doVerify(start_addr, FLASH_WRITE_SIZE, (uint32_t *)data_buf, &poFlashStatusWord) != Fapi_Status_Success) {
            Fapi_flushPipeline();
            asm_busy_wait(FSM_DELAY_LOOPS);
            return BOOT_INT_FLASH_WRITE_ERR;
        }

        *start_addr += FLASH_WRITE_SIZE;
    }

    /*
    * F021 Flash API Reference Guide (SPNU501H)
    * The pipeline must be flushed before the first non-API Flash read after an operation that
    * modifies the Flash contents (Erasing and Programming).
    *
    * The Fapi_doVerify() function should include a call to Fapi_flushPipeline() but it is not well documented
    * in the TRM. We flush the pipeline manually to be sure
    */
    Fapi_flushPipeline();
    asm_busy_wait(FSM_DELAY_LOOPS);

    return BOOT_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Enables relevant sectors of slot for erase/programming
 *
 * @return false if enabling sectors did not finish within timeout
 */
static bool boot_fapi_enable_sectors(uint8_t slot_num) {
    if (slot_num == 0 || slot_num > 5) {
        return false;
    }

    switch (slot_num) {
    case FLASH_SLOT_BOOT_A:
        Fapi_enableMainBankSectors(0x0006); // Sector 1 & 2
        break;

    case FLASH_SLOT_BOOT_B:
        Fapi_enableMainBankSectors(0x0018); // Sector 3 & 4
        break;

    case FLASH_SLOT_APP_CORE:
        Fapi_enableMainBankSectors(0x00C0); // Sector 6 & 7
        break;

    case FLASH_SLOT_APP_EXT_A:
        Fapi_enableMainBankSectors(0x0300); // Sector 8 & 9
        break;

    case FLASH_SLOT_APP_EXT_B:
        Fapi_enableMainBankSectors(0x0C00); // Sector 10 & 11
        break;

    default:
        return false;
    }

    uint32_t num_loops = 0;

    while (FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady) {
        num_loops++;

        if (num_loops > FAPI_TIMEOUT_LOOPS) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Issues erase command and verifies that flash was actually erased
 *
 * @return false if erasing sectors did not finish within timeout or erase failed
 */
static bool boot_fapi_sector_erase(uint32_t sector1_addr, uint32_t sector1_len, uint32_t sector2_addr, uint32_t sector2_len) {

    Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32_t *) sector1_addr);

    if (!boot_fapi_check_erase(sector1_addr, sector1_len)) {
        return false;
    }

    Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32_t *) sector2_addr);

    if (!boot_fapi_check_erase(sector2_addr, sector2_len)) {
        return false;
    }

    /*
    * F021 Flash API Reference Guide (SPNU501H)
    * The pipeline must be flushed before the first non-API Flash read after an operation that
    * modifies the Flash contents (Erasing and Programming).
    *
    * The Fapi_doBlankCheck() function should include a call to Fapi_flushPipeline() but it is not well documented
    * in the TRM. We flush the pipeline manually to be sure
    */
    Fapi_flushPipeline();
    asm_busy_wait(FSM_DELAY_LOOPS);

    return true;
}

/**
 * @brief Verifies that erased sectors are actually blank
 *
 * @return false if non-blank region found
 */
static inline bool boot_fapi_check_erase(uint32_t sector_addr, uint32_t sector_len) {
    if (!boot_fapi_poll_fsm_status()) {
        return false;
    }

    /*
    * F021 Flash API Reference Guide (SPNU501H)
    *
    * The read functions do not combine main and ECC ranges. Because of this, if you need a read operation
    * performed on the data and corresponding ECC, you must call the function for both. For example, if you
    * have erased Bank 0, Sector 0, you would need to perform a blank check on Sector 0 main address range
    * and ECC address range.
    *
    * TODO: ALEA-3108 Implement blank check on ECC range
    */

    Fapi_FlashStatusWordType poFlashStatusWord;
    return (Fapi_doBlankCheck((uint32_t *) sector_addr, sector_len, &poFlashStatusWord) == Fapi_Status_Success);
}

static inline bool boot_fapi_poll_fsm_status(void) {
    uint32_t num_loops = 0;

    // Wait for FSM to finish operation
    while (FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy) {
        num_loops++;

        if (num_loops > FAPI_TIMEOUT_LOOPS) {
            boot_fapi_reset_fsm();
            return false;
        }
    }

    num_loops = 0;

    while (FAPI_GET_FSM_STATUS != Fapi_Status_Success) {
        num_loops++;

        if (num_loops > FAPI_TIMEOUT_LOOPS) {
            boot_fapi_reset_fsm();
            return false;
        }
    }

    return true;
}

/**
 * @brief Suspends the FSM and clears registers in order to branch back to obc_main() eventually
 * without causing a prefetch abort. Only program and erase are valid suspendable operations
 */
static void boot_fapi_reset_fsm(void) {
    FAPI_SUSPEND_FSM;
    asm_busy_wait(FSM_DELAY_LOOPS);
    Fapi_issueAsyncCommand(Fapi_ClearMore);
    asm_busy_wait(FSM_DELAY_LOOPS);
    /* It is not well documented if this is required upon a failed erase/write
     * There is no loss in doing this so we defensively do it anyway
     */
    Fapi_flushPipeline();
    asm_busy_wait(FSM_DELAY_LOOPS);
}
