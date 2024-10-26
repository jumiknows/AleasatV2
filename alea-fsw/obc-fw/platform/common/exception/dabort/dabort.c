/**
 * @file dabort.h
 * @brief Helper functions to manage data aborts
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "dabort.h"

// HALCoGen
#include "reg_flash.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define FEDACSTATUS_B1_UNC_ERR_BIT 8U

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

volatile uint32_t DABORT_FUNC_ERR_FATAL = 1;
volatile uint32_t DABORT_FUNC_ERR_FLAG  = 0;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void dabort_FUNC_ERR_set_nonfatal(void) {
    dabort_FUNC_ERR_clear();
    DABORT_FUNC_ERR_FATAL = 0;
}

void dabort_FUNC_ERR_set_fatal(void) {
    DABORT_FUNC_ERR_FATAL = 1;
    dabort_FUNC_ERR_clear();
}

bool dabort_FUNC_ERR_get(void) {
    return (DABORT_FUNC_ERR_FLAG != 0);
}

uint32_t dabort_FUNC_ERR_clear(void) {
    DABORT_FUNC_ERR_FLAG = 0;

    // Errors are frozen until the Uncorrectable Error Address is read
    volatile uint32_t FUNC_ERR_addr = 0U;
    FUNC_ERR_addr = flashWREG->FUNCERRADD;
    return FUNC_ERR_addr;
}
