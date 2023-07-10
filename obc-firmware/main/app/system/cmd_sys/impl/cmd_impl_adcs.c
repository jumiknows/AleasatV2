/**
 * @file cmd_impl_adcs.c
 * @brief Implementation of commands related to ADCS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// WMM
#include "wmm.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the world magnetic model (wmm)
 *
*/
cmd_sys_resp_code_t cmd_impl_WMM_INIT(const cmd_sys_cmd_t *cmd) {

    wmm_init();

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Checks if wmm has been intialized
 *
*/
cmd_sys_resp_code_t cmd_impl_WMM_IS_INIT(const cmd_sys_cmd_t *cmd, cmd_WMM_IS_INIT_resp_t *resp) {

    resp->is_init = wmm_is_init();

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Get the magentic reference vector (check code docs for wmm_get_mag_ref)
 *
*/
cmd_sys_resp_code_t cmd_impl_WMM_GET_MAG_REF(const cmd_sys_cmd_t *cmd, cmd_WMM_GET_MAG_REF_args_t *args, cmd_WMM_GET_MAG_REF_resp_t *resp) {
    float32_t time_years = wmm_get_date(args->year, args->month, args->day);

    if(wmm_get_mag_ref(args->lat, args->lon, args->alt, time_years, resp->mag_ref)){
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }
}
