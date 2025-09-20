/**
 * @file obc_main.c
 * @brief Entry point for the application
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_main.h"

// Common
#include "fw_structs.h"
#include "fw_memmap.h"
#include "asm_utils.h"
#include "obc_crc.h"

// Platform
#include "dabort.h"

// HALCoGen
#include "gio.h"
#include "reg_gio.h"
#include "system.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#ifdef PLATFORM_ALEA_V1

#define LED_PORT gioPORTA
#define LED_PIN  7

#elif PLATFORM_LAUNCHPAD_1224

#define LED_PORT gioPORTB
#define LED_PIN  1

#endif

#define LED_BLINK_MS     100U
#define LED_BLINK_LOOPS (LED_BLINK_MS * 1000U * ((uint32_t)GCLK_FREQ) / 5U) // 5 is the number of cycles / loop

/* The following symbols are defined at link time (see main/linker.cmd)
 * Locations and size of asm_utils
 */
extern unsigned int asm_utils_load;
extern unsigned int asm_utils_run;
extern unsigned int asm_utils_size;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void obc_main(void) {
    // The asm_utils.asm functions are moved to RAM to ensure consistent timing behaviour
    memcpy(&asm_utils_run, &asm_utils_load, (uint32_t)&asm_utils_size);

    // This function may result in data aborts due to ECC errors
    // Disable interrupts so we can easily catch and identify these data aborts
    _disable_interrupts(); // compiler intrinsic

    // Hardware initialization
    gioInit();

    // TODO: ALEA-3092 Branch to firmware using burn number instead of by sequence of slot

    while (1) {
        // Start at 1 because 0 is the startup slot
        for (uint32_t i = 1; i < FLASH_SLOT_COUNT; i++) {
            // Flash the LED
            gioSetBit(LED_PORT, LED_PIN, 1);
            asm_busy_wait(LED_BLINK_LOOPS / 2);
            gioSetBit(LED_PORT, LED_PIN, 0);
            asm_busy_wait(LED_BLINK_LOOPS * 2);

            // Since we're reading from flash outside of our own flash slot we might encounter uncorrectable flash (ECC) errors.
            // We don't want these errors to be fatal (if it happens we move on to the next flash slot),
            // so we set a flag that allows flash ECC data aborts to occur (while recording that it happened).
            dabort_FUNC_ERR_set_nonfatal();

            do {
                const volatile fw_structs_t *structs = FW_STRUCTS[i];

                // Read header magic
                uint32_t headerMagic = structs->header.magic;

                // Check for a flash ECC error early (a fully erased flash slot might trigger this)
                if (dabort_FUNC_ERR_get()) {
                    dabort_FUNC_ERR_clear(); // TODO check address is as expected
                    break;
                }

                // Check header magic
                if (headerMagic != FW_HEADER_MAGIC) {
                    break;
                }

                // Check # of previous boots
                // TODO: ALEA-3093

                // Check CRC32
                uint32_t calc_crc32 = crc_32_buf(CRC32_SEED, (uint8_t *)&structs->entrypoint, structs->header.size);

                if (calc_crc32 != structs->header.crc32) {
                    break;
                }

                // Check entrypoint magic
                if (structs->entrypoint.magic != FW_ENTRYPOINT_MAGIC) {
                    break;
                }

                // Check entrypoint flash addr
                if (structs->entrypoint.flash_addr != (uint32_t)structs) {
                    break;
                }

                // Check for flash ECC errors one more time
                if (dabort_FUNC_ERR_get()) {
                    dabort_FUNC_ERR_clear();
                    break;
                }

                // At this point we've validated the firmware image so any flash ECC errors are real and should be fatal
                dabort_FUNC_ERR_set_fatal();

                // TODO deinitialize hardware

                // Try to boot
                uint32_t jump_instr_addr = (uint32_t) & (structs->entrypoint.jump_instr);
                asm_jump_to_addr(jump_instr_addr);

                // Nothing after the above call should ever execute

            } while (0);

            dabort_FUNC_ERR_set_fatal();
        }
    }
}
