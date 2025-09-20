/**
 * @file telem_impl.c
 * @brief Implementation of telem units
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "telem_gen.h"
#include "telem_error.h"
#include "telem.h"

// OBC
#include "obc_rtc.h"
#include "obc_time.h"
#include "logger.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

telem_err_t telem_impl_TEST_GET_EPOCH(telem_TEST_GET_EPOCH_resp_t *resp) {
    resp->epoch = rtc_get_epoch_time();
    return TELEM_SUCCESS;
}
