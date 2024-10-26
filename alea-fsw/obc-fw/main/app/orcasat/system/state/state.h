/** @file state.h
 *   @brief State Handler
 *   @author Alireza Alidousti
 *   @date Sep 23, 2019
 */

#ifndef STATE_H_
#define STATE_H_

#include "stdbool.h"

/** @typedef state_err_enum_t
 *   @brief state handler error codes
 */
typedef enum {
    STATE_SUCCESS                      = 0x00, /**<  Operation successfully completed                              */
    STATE_INVALID_TRANS_REQ            = 0x01, /**<  Transition is not enabled in the current state */
    STATE_HANDLER_TASK_NOT_INITIALIZED = 0x02, /**<  State handler task has not been initialized                   */
    STATE_PENDING_NOTIFICATION         = 0x03, /**<  State handler task has not processed a previous notification  */
    STATE_FAILED_TO_CREATE_MUTEX       = 0x04, /**<  Failed to initialize the state-mutex during initialization    */
    STATE_FAILED_TO_TAKE_MUTEX         = 0x05, /**<  Failed to aquire state-mutex */
} state_err_enum_t;

/** @typedef state_transition_id_t
 *   @brief state handler transition ids
 */
typedef enum {
    OBC_TEMP_TOO_LOW             = 0x01,
    OBC_TEMP_TOO_HIGHT           = 0x02,
    EXIT_LOW_POW_STATE           = 0x03,
    RTC_ERR_DETECTED             = 0x04,
    FS_CRITICAL_IEC_CND          = 0x05,
    COMMAND_TRANS                = 0x06,
    GCS_TIMEOUT                  = 0x07,
    POW_BAD_DURING_TRANSMIT      = 0x08,
    INCONSISTENT_FLAG_FILES      = 0x09,
    CHIME_PAYLOAD_TASK_START     = 0x0A,
    CHIME_PAYLOAD_TASK_COMPLETE  = 0x0B,
    ALTAIR_PAYLOAD_TASK_START    = 0x0C,
    ALTAIR_PAYLOAD_TASK_COMPLETE = 0x0D,
} state_transition_id_t;

/** @typedef state_t
 *   @brief OBC state
 */
typedef enum {
    SAFE   = 0x0,
    READY  = 0x1,
    SLEEP  = 0x2,
    CHIME  = 0x3,
    ALTAIR = 0x4,
} state_t;

state_err_enum_t init_state_handler(void);
state_err_enum_t get_current_state(state_t *state);
state_err_enum_t send_event(state_transition_id_t transition_id);

// The following functions modify the state transition table
state_err_enum_t enable_transition(state_transition_id_t transition_id);
state_err_enum_t disable_transition(state_transition_id_t transition_id);
state_err_enum_t enable_transition_from_state(state_transition_id_t transition_id, state_t state);
state_err_enum_t disable_transition_from_state(state_transition_id_t transition_id, state_t state);

#endif // STATE_H_
