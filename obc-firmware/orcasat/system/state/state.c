/**
 * @file
 * @brief State Handler
 * @author Alireza Alidousti
 * @date Sep 23, 2019
 */

#include "state.h"
#include "rtos.h"
#include "logger.h"
#include "obc_task_info.h"

// ------------------------------ DEFENITIONS ---------------------------------

/** @typedef state_transition_t
 *   @brief a transition table row
 */
typedef struct {
    uint32_t transition_id : 8;        /**<  Transition Id                         */
    uint32_t enabled : 1;              /**<  Transition is currently enabled       */
    uint32_t end_state : 3;            /**<  Next state of the OBC                 */
    uint32_t valid_trigger_states : 5; /**<  Valid start states for the transition */
} state_transition_t;

typedef struct {
    uint32_t transition_id : 8;
    uint32_t enabled : 1;
    uint32_t valid_trigger_states : 5;
    uint32_t current_state : 3;
} invalid_transition_log_t;

#define STATE_ENABLED  0b1
#define STATE_DISABLED 0b0

#define NUM_DEFAULT_STATE_TRANSISIONS 13

#define ANY_TRIGGER_STATE    0b11111
#define SAFE_TRIGGER_STATE   0b00001
#define READY_TRIGGER_STATE  0b00010
#define SLEEP_TRIGGER_STATE  0b00100
#define CHIME_TRIGGER_STATE  0b01000
#define ALTAIR_TRIGGER_STATE 0b10000

state_transition_t DEFAULT_STATE_TRANSISION_TABLE[NUM_DEFAULT_STATE_TRANSISIONS] = {
    {.transition_id = OBC_TEMP_TOO_LOW, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = READY_TRIGGER_STATE},
    {.transition_id = OBC_TEMP_TOO_HIGHT, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = READY_TRIGGER_STATE},
    {.transition_id = EXIT_LOW_POW_STATE, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = SLEEP_TRIGGER_STATE},
    {.transition_id = RTC_ERR_DETECTED, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = ANY_TRIGGER_STATE},
    {.transition_id = FS_CRITICAL_IEC_CND, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = ANY_TRIGGER_STATE},
    {.transition_id = COMMAND_TRANS, .enabled = STATE_ENABLED, .end_state = READY, .valid_trigger_states = SAFE_TRIGGER_STATE},
    {.transition_id = GCS_TIMEOUT, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = ANY_TRIGGER_STATE},
    {.transition_id = POW_BAD_DURING_TRANSMIT, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = ANY_TRIGGER_STATE},
    {.transition_id = INCONSISTENT_FLAG_FILES, .enabled = STATE_ENABLED, .end_state = SAFE, .valid_trigger_states = READY_TRIGGER_STATE},
    {.transition_id = CHIME_PAYLOAD_TASK_START, .enabled = STATE_ENABLED, .end_state = CHIME, .valid_trigger_states = READY_TRIGGER_STATE},
    {.transition_id = CHIME_PAYLOAD_TASK_COMPLETE, .enabled = STATE_ENABLED, .end_state = READY, .valid_trigger_states = CHIME_TRIGGER_STATE},
    {.transition_id = ALTAIR_PAYLOAD_TASK_START, .enabled = STATE_ENABLED, .end_state = ALTAIR, .valid_trigger_states = READY_TRIGGER_STATE},
    {.transition_id = ALTAIR_PAYLOAD_TASK_COMPLETE, .enabled = STATE_ENABLED, .end_state = READY, .valid_trigger_states = ALTAIR_TRIGGER_STATE},
};

const TickType_t xDelay1s = pdMS_TO_TICKS(1000UL);

// --------------------------------- FUN DEF ----------------------------------

void vStateHandlerTask(void* pvParameters);
state_transition_t* get_state_transition(state_transition_id_t transition_id);
bool valid_trigger_state(state_transition_t* state_transition);
void make_state_transition(state_transition_id_t transition_id);

// ---------------------------------- STATE -----------------------------------

state_transition_t* state_transition_table;
state_t current_state;

TaskHandle_t xStateHandlerTaskHandle = NULL;
StaticTask_t xStateHandlerTaskBuffer;
StackType_t xStateHandlerTaskStack[STATE_HANDLER_STACK_DEPTH];

SemaphoreHandle_t xStateMutex = NULL;
StaticSemaphore_t xStateMutexBuffer;

// --------------------------------- API --------------------------------------

/**
 * @brief Initializes:
 * - state handler task
 * - state mutex
 * - current state variable
 * - state transition table
 *
 * @return STATE_SUCCESS if it can successfully initialize all the components
 */
state_err_enum_t init_state_handler(void) {
    xStateMutex = xSemaphoreCreateBinaryStatic(&xStateMutexBuffer);

    if (xStateMutex == NULL) {
        return STATE_FAILED_TO_CREATE_MUTEX;
    }

    xStateHandlerTaskHandle = xTaskCreateStatic(&vStateHandlerTask, "STATE_HANDLER", STATE_HANDLER_STACK_DEPTH, NULL, STATE_HANDLER_TASK_PRIORITY, xStateHandlerTaskStack, &xStateHandlerTaskBuffer);

    current_state          = SLEEP;
    state_transition_table = DEFAULT_STATE_TRANSISION_TABLE;
    // TODO: Load the latest transition table from non volatile memory.

    return STATE_SUCCESS;
}

/**
 * @brief Get the current state of the OBC.
 *
 * @param state: pointer to space in memory to store the current-state
 * @return STATE_SUCCESS if it can copy the current state into the provided memory space.
 */
state_err_enum_t get_current_state(state_t* state) {
    if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
        *state = current_state;
        xSemaphoreGive(xStateMutex);
        return STATE_SUCCESS;
    } else {
        return STATE_FAILED_TO_TAKE_MUTEX;
    }
}

/**
 * @brief Sends a notification to the State Hanndler task to trigger a state-transition.
 *
 * @param transition_id: identifies a row in the state transition table
 * @return returns STATE_SUCCESS except when the,
 * - state handler task has not been initialized (STATE_HANDLER_TASK_NOT_INITIALIZED)
 * - state handler task has not processed a previous notification (STATE_PENDING_NOTIFICATION)
 */
state_err_enum_t send_event(state_transition_id_t transition_id) {
    if (xStateHandlerTaskHandle == NULL) {
        return STATE_HANDLER_TASK_NOT_INITIALIZED;
    }

    BaseType_t error = xTaskNotify(xStateHandlerTaskHandle, (uint32_t)transition_id, eSetValueWithoutOverwrite);

    if (error != pdPASS) {
        return STATE_PENDING_NOTIFICATION;
    }

    return STATE_SUCCESS;
}

/**
 * @brief Enables a transition defined in the transition table
 *
 * @param transition_id: identifies a row in the state transition table
 * @return STATE_SUCCESS if it can successfully enable a transition
 */
state_err_enum_t enable_transition(state_transition_id_t transition_id) {
    state_transition_t* state_transition = get_state_transition(transition_id);
    if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
        state_transition->enabled = STATE_ENABLED;
        xSemaphoreGive(xStateMutex);
        return STATE_SUCCESS;
    } else {
        return STATE_FAILED_TO_TAKE_MUTEX;
    }
}

/**
 * @brief Disables a transition defined in the transition table
 *
 * @param transition_id: identifies a row in the state transition table
 * @return STATE_SUCCESS if it can successfully disable a transition
 */
state_err_enum_t disable_transition(state_transition_id_t transition_id) {
    state_transition_t* state_transition = get_state_transition(transition_id);
    if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
        state_transition->enabled = STATE_DISABLED;
        xSemaphoreGive(xStateMutex);
        return STATE_SUCCESS;
    } else {
        return STATE_FAILED_TO_TAKE_MUTEX;
    }
}

/**
 * @brief Enable transition from state.
 *
 * @param transition_id: identifies a row in the state transition table
 * @param state: state to be enabled
 * @return STATE_SUCCESS if it can successfully enable a from state
 */
state_err_enum_t enable_transition_from_state(state_transition_id_t transition_id, state_t state) {
    state_transition_t* state_transition = get_state_transition(transition_id);
    if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
        state_transition->valid_trigger_states |= (0b00001 << (uint8_t)state);
        xSemaphoreGive(xStateMutex);
        return STATE_SUCCESS;
    } else {
        return STATE_FAILED_TO_TAKE_MUTEX;
    }
}

/**
 * @brief Disable transition from state.
 *
 * @param transition_id: identifies a row in the state transition table
 * @param state: state to be disabled
 * @return STATE_SUCCESS if it can successfully disable a from state
 */
state_err_enum_t disable_transition_from_state(state_transition_id_t transition_id, state_t state) {
    state_transition_t* state_transition = get_state_transition(transition_id);
    if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
        state_transition->valid_trigger_states &= (uint8_t)(~(0b00001 << (uint8_t)state));
        xSemaphoreGive(xStateMutex);
        return STATE_SUCCESS;
    } else {
        return STATE_FAILED_TO_TAKE_MUTEX;
    }
}

// ----------------------------------- HELPERS --------------------------------

/**
 * @brief Responsible for managing the current state, and state transition table.
 * - Blocks on state-transition notifications.
 * - Expects the notification value to be a state_transition_id_t.
 */
void vStateHandlerTask(void* pvParameters) {
    state_transition_id_t transition_id;
    xSemaphoreGive(xStateMutex);

    for (;;) {
        transition_id = (state_transition_id_t)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (xSemaphoreTake(xStateMutex, xDelay1s) == pdTRUE) {
            make_state_transition(transition_id);
            xSemaphoreGive(xStateMutex);
        } else {
            log_str(INFO, LOG_PRINT_GENERAL, true, "state failed to take mutex");
        }
    }
}

/**
 * @brief Finds the transition with a matching transition_id.
 *
 * @param transition_id: identifies a row in the state transition table
 * @return a pointer to a row in the state transition table with the matching transition_id
 */
state_transition_t* get_state_transition(state_transition_id_t transition_id) {
    uint8_t state_transition_idx = ((uint8_t)transition_id) - 1;
    return (state_transition_t*)(state_transition_table + state_transition_idx);
}

/**
 * @brief Validates the trigger state of a requested transition.
 *
 * @param state_transition: pointer to a row in the state transition table
 * @return true only if current state is in the valid trigger states
 */
bool valid_trigger_state(state_transition_t* state_transition) {
    return (bool)((state_transition->valid_trigger_states >> ((uint8_t)current_state)) & 0b00000001);
}

/**
 * @brief Updates the current state only if the transition is enabled and we are currently in a
 * valid trigger state.
 *
 * @param transition_id: identifies a row in the state transition table
 */
void make_state_transition(state_transition_id_t transition_id) {
    state_transition_t* state_transition = get_state_transition(transition_id);
    if ((valid_trigger_state(state_transition)) && (state_transition->enabled == STATE_ENABLED)) {
        current_state = (state_t)state_transition->end_state;
        log_str(INFO, LOG_PRINT_GENERAL, true, "current_state = %i", (uint8_t)current_state);
    } else {
        invalid_transition_log_t invalid_transition_log_data = {
            .transition_id        = transition_id,
            .enabled              = state_transition->enabled,
            .valid_trigger_states = state_transition->valid_trigger_states,
            .current_state        = current_state,
        };

        log_data(INFO, LOG_PRINT_GENERAL, true, sizeof(invalid_transition_log_t), (void*)(&invalid_transition_log_data));
    }
}
