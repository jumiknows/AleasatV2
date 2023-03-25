/**
 * @file cmd_impl_test_dsp.c
 * @brief Implementation of test commands related to CMSIS-DSPLIB
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// DSPLIB
#include "arm_math.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_TEST_DSPLIB(const cmd_sys_cmd_t *cmd, cmd_TEST_DSPLIB_args_t *args, cmd_TEST_DSPLIB_resp_t *resp) {
    float32_t input[2] = {
        args->x,
        args->y,
    };

    float32_t output[2] = {0.0, 0.0};

    arm_abs_f32(input, output, 2);

    resp->x_abs = output[0];
    resp->y_abs = output[1];

    return CMD_SYS_RESP_CODE_SUCCESS;
}
