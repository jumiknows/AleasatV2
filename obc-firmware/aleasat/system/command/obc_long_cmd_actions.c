/** @file obc_long_cmd_actions.c
 *  @brief OBC long command function implementations.
 *
 *  TODO(ALEA-574): Store long commands and have a command to get them back.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_cmd.h"
#include "obc_long_cmd_actions.h"
#include "obc_task_info.h"
#include "obc_temperature.h"
#include "obc_utils.h"
#include "obc_cmd.h"
#include "obc_rtos.h"
#include "logger.h"
#include "rtc_state.h"
#include "scheduler.h"

// Standard Library
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

// ------------------------ RTOS LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints information about all RTOS tasks.
 */
void cmd_rtos_tasks(uint32_t arg_len, void* arg) {
    print_tasks();
}

/** @brief Prints info about the OBC RTOS layer. */
void cmd_rtos_info(uint32_t arg_len, void* arg) {
    print_rtos_status();
}

// ------------------------ RTC LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints RTC config information.
 */
void cmd_rtc_info(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        print_rtc_info();
    }
}

/**
 * @brief Prints RTC stats.
 */
void cmd_rtc_stats(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        print_rtc_stats();
    }
}

// ------------------------ SCHEDULE LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints the schedule.
 */
void cmd_print_sched(uint32_t arg_len, void* arg) {
    print_schedule();
}

// ------------------------ TESTING LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Executes the temperature sensor self-test routine.
 */
void cmd_test_temperature(uint32_t arg_len, void* arg) {
    self_test_temperature();
}

void cmd_test_log_str(uint32_t arg_len, void* arg) {
    char test_payload[MAX_PAYLOAD_SIZE] = "";

    log_str(DEBUG, LOG_PRINT_GENERAL, false, "A");

    for (uint8_t i = 0; i < MAX_PAYLOAD_SIZE; i++) {
        strcat(test_payload, "A");
    }

    log_str(DEBUG, LOG_PRINT_GENERAL, false, test_payload);
}
