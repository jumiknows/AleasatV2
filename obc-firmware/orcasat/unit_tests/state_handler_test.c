#include "state.h"
#include "logger.h"
#include "unit_tests.h"
#include "obc_task_info.h"

void vStateHandlerTestTask(void* pvParameters);
void emit_transition_event(state_transition_id_t transition_id, TickType_t delay, const char* exp_end_state);

void init_state_handler_test(void) {
    state_err_enum_t err = init_state_handler();

    if (err != STATE_SUCCESS) {
        log_str(ERROR, PRINT_GENERAL, true, "state handler failed");
        return;
    }

    xTaskCreate(&vStateHandlerTestTask, "state handler test", configMINIMAL_STACK_SIZE, NULL, STATE_HANDLER_TEST_TASK_PRIORITY, NULL);
}

void vStateHandlerTestTask(void* pvParameters) {
    const TickType_t xDelay100ms = pdMS_TO_TICKS(100UL);
    const TickType_t xDelay2s    = pdMS_TO_TICKS(2000UL);
    TaskHandle_t xHandle;
    TaskStatus_t xTaskDetails;

    // current_state = SLEEP
    emit_transition_event(EXIT_LOW_POW_STATE, xDelay100ms, "SAFE");
    // current_state = SAFE
    emit_transition_event(COMMAND_TRANS, xDelay100ms, "READY");
    // current_state = READY
    emit_transition_event(CHIME_PAYLOAD_TASK_START, xDelay100ms, "CHIME");
    // current_state = CHIME
    emit_transition_event(CHIME_PAYLOAD_TASK_COMPLETE, xDelay100ms, "READY");
    // current_state = READY

    while (true) {
        xHandle = xTaskGetHandle("STATE_HANDLER");
        vTaskGetInfo(xHandle, &xTaskDetails, pdFALSE, eInvalid);
        log_str(INFO, PRINT_GENERAL, false, "STATE: %i", (uint8_t)xTaskDetails.eCurrentState);
        // current_state = READY
        emit_transition_event(OBC_TEMP_TOO_LOW, xDelay2s, "SAFE");
        // current_state = SAFE
        emit_transition_event(COMMAND_TRANS, xDelay2s, "READY");
        // current_state = READY
    }
}

void emit_transition_event(state_transition_id_t transition_id, TickType_t delay, const char* exp_end_state) {
    vTaskDelay(delay);
    log_str(INFO, PRINT_GENERAL, true, "Transitioning to %s", exp_end_state);
    state_err_enum_t err = send_event(transition_id);
    if (err != STATE_SUCCESS) {
        log_str(ERROR, PRINT_GENERAL, true, "Transition FAILED");
    }
}
