/**
 * @file startup_crt.c
 * @brief Initialization of the C runtime environment for the execution of a
 *        C application. The device is assumed to already be initialized
 *        (clocks configured, PLL locked, self-tests passed, etc.)
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Startup
#include "crt.h"

// Exceptions
#include "exc_ram.h"

// HALCoGen
#include "esm.h"

#include "sys_core.h"
#include "sys_selftest.h"
#include "sys_vim.h"

// Runtime Support Library
#include "cpy_tbl.h"

// Standard Library
#include <stdlib.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void _c_int01(void) {
    // Initialize Core Registers to avoid CCM Error
    _coreInitRegisters_();

    // Initialize Stack Pointers
    _coreInitStackPointer_();

    // Initialize CPU RAM using hardware auto-initialization
    _coreDisableRamEcc_();
    memoryInit(0x1U);
    _coreEnableRamEcc_();

    // Copy RAM exception vector table from flash to RAM
    copy_in(&EXCRAM_COPY_TABLE);

    // Initialize VIM table
    vimInit();

    // Configure system response to error conditions signaled to the ESM group1
    // This function can be configured from the ESM tab of HALCoGen
    esmInit();

    // Initialize global variables
    __TI_auto_init();

    // Call the application
    main();

    // Should never reach here
    exit(0);
}
