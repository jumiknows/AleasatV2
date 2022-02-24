#include "scheduler.h"
#include "logger.h"
#include "obc_task_info.h"
#include "obc_rtc.h"
#include "unit_tests.h"
#include "rtos_task.h"
#include "obc_watchdog.h"

// --------------------------- Time Constants ---------------------------------

#define MINUTE_TO_SEC 60
#define HOUR_TO_SEC   (60 * MINUTE_TO_SEC)
#define DAY_TO_SEC    (24 * HOUR_TO_SEC)
#define YEAR_TO_SEC   (356 * DAY_TO_SEC)
uint32_t DAYS_OF_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// ------------------------ Periodic Schedule ---------------------------------

typedef struct {
    task_id_t task_id;
    real_time_t start_time;
    uint32_t period;
    uint32_t* exec_count;
} periodic_schedule_t;

#define PERIODIC_TASK_ARGUMENT_SIZE sizeof(periodic_schedule_t)

// ------------------------ Action Schedule ---------------------------------

typedef struct {
    task_id_t task_id;
    real_time_t exec_time;
    uint32_t* exec_count;
} action_schedule_t;

#define ACTION_TASK_ARGUMENT_SIZE sizeof(action_schedule_t)

// --------------------------- Periodic Tasks ---------------------------------

#define TASK_A_ID 1
#define TASK_B_ID 2

real_time_t START_TIME = {
    year: 0,
    month: 0,
    day: 0,
    hour: 0,
    minute: 0,
    second: 10,
    epoch: 10,
};
uint32_t TASK_A_PERIOD = 2; // 2s
uint32_t TASK_B_PERIOD = 5; // 5s

uint32_t TASK_A_EXEC_COUNT = 0;
uint32_t TASK_B_EXEC_COUNT = 0;

// ----------------------------- Action Tasks ---------------------------------

#define TASK_C_ID 3
real_time_t EXEC_TIME = {
    year: 0,
    month: 0,
    day: 0,
    hour: 0,
    minute: 0,
    second: 20,
    epoch: 20,
};

uint32_t TASK_C_EXEC_COUNT = 0;

// ---------------------------- Function Declaration --------------------------

int32_t to_sec(real_time_t* t);
void test_periodic_task_execution_time(uint32_t argument_size, void* argument);
void test_action_task_execution_time(uint32_t argument_size, void* argument);
void vSchedulerTestTask(void* pvParameters);

// ------------------------------------ API -----------------------------------

void init_scheduler_test(void) {
    xTaskCreate(&vSchedulerTestTask, "scheduler_test", configMINIMAL_STACK_SIZE * 10, NULL, SCHEDULER_TEST_TASK_PRIORITY, NULL);
}

// ---------------------------------- Helpers ---------------------------------

void vSchedulerTestTask(void* pvParameters) {
    log_str(INFO, PRINT_GENERAL, false, "SCHEDULER TEST");
    periodic_schedule_t task_a_schedule = {
        task_id: TASK_A_ID,
        start_time: START_TIME,
        period: TASK_A_PERIOD,
        exec_count: &TASK_A_EXEC_COUNT,
    };

    periodic_schedule_t task_b_schedule = {
        task_id: TASK_B_ID,
        start_time: START_TIME,
        period: TASK_B_PERIOD,
        exec_count: &TASK_B_EXEC_COUNT,
    };

    action_schedule_t task_c_schedule = {
        task_id: TASK_C_ID,
        exec_time: EXEC_TIME,
        exec_count: &TASK_C_EXEC_COUNT,
    };

    scheduler_err_t errcode = scheduler_start();
    if (errcode != SCHEDULER_SUCCESS) {
        log_str(ERROR, PRINT_GENERAL, true, "scheduler start failed");
    } else {
        // TASK A
        log_str(INFO, PRINT_GENERAL, false, "scheduling task A");
        scheduler_schedule_action(NULL, INTERNAL, START_TIME, TASK_A_PERIOD, &test_periodic_task_execution_time, PERIODIC_TASK_ARGUMENT_SIZE, &task_a_schedule);
        // TASK B
        log_str(INFO, PRINT_GENERAL, false, "scheduling task B");
        scheduler_schedule_action(NULL, INTERNAL, START_TIME, TASK_B_PERIOD, &test_periodic_task_execution_time, PERIODIC_TASK_ARGUMENT_SIZE, &task_b_schedule);
        // TASK C
        log_str(INFO, PRINT_GENERAL, false, "scheduling task C");
        scheduler_schedule_action(NULL, INTERNAL, EXEC_TIME, 0, &test_action_task_execution_time, ACTION_TASK_ARGUMENT_SIZE, &task_c_schedule);
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        log_str(INFO, PRINT_GENERAL, false, "10s passed");
    }
}

void test_action_task_execution_time(uint32_t argument_size, void* argument) {
    action_schedule_t* schedule = (action_schedule_t*)argument;
    uint32_t* exec_count        = schedule->exec_count;

    *exec_count = *exec_count + 1;
    if (*exec_count != 1) {
        log_str(ERROR, PRINT_GENERAL, true, "%2d: exec more than once", schedule->task_id);
        return;
    }

    real_time_t current_time = {0};
    rtc_err_t errcode        = rtc_get_current_time(&current_time);
    if (errcode != RTC_NO_ERR) {
        log_str(ERROR, PRINT_GENERAL, true, "%2d: can't get current time", schedule->task_id);
        return;
    }

    int32_t curr_time_sec = to_sec(&current_time);
    int32_t exec_time_sec = to_sec(&schedule->exec_time);
    int32_t delta_sec     = curr_time_sec - exec_time_sec;
    if (delta_sec == 0) {
        log_str(INFO, PRINT_GENERAL, false, "%2d: successful execution", schedule->task_id);
    } else {
        log_str(INFO, PRINT_GENERAL, false, "%2d: invalid execution time", schedule->task_id);
    }

    return;
}

void test_periodic_task_execution_time(uint32_t argument_size, void* argument) {
    periodic_schedule_t* schedule = (periodic_schedule_t*)argument;
    uint32_t* exec_count          = schedule->exec_count;

    real_time_t current_time = {0};
    rtc_err_t errcode        = rtc_get_current_time(&current_time);
    if (errcode != RTC_NO_ERR) {
        log_str(ERROR, PRINT_GENERAL, true, "%2d: can't get current time", schedule->task_id);
        return;
    }

    int32_t curr_time_sec  = to_sec(&current_time);
    int32_t start_time_sec = to_sec(&schedule->start_time);
    int32_t delta_sec      = curr_time_sec - start_time_sec;
    if (delta_sec < 0) {
        log_str(ERROR, PRINT_GENERAL, true, "%2d: exec before start time", schedule->task_id);
        return;
    }

    if ((delta_sec % schedule->period) <= 1) {
        log_str(INFO, PRINT_GENERAL, false, "%2d: successful execution", schedule->task_id);
    } else {
        log_str(INFO, PRINT_GENERAL, false, "%2d: invalid execution time", schedule->task_id);
    }

    *exec_count                  = *exec_count + 1;
    uint32_t expected_exec_count = (delta_sec / schedule->period) + 1;
    if (*exec_count != expected_exec_count) {
        log_str(INFO, PRINT_GENERAL, false, "%2d: exp %d, act %d", schedule->task_id, expected_exec_count, *exec_count);
    }

    return;
}

int32_t to_sec(real_time_t* t) {
    return (t->year * YEAR_TO_SEC) + (t->month == 1 ? 0 : DAYS_OF_MONTH[t->month - 1] * DAY_TO_SEC) + (t->day == 1 ? 0 : t->day * DAY_TO_SEC) + (t->hour * HOUR_TO_SEC) + (t->minute * MINUTE_TO_SEC) +
           t->second;
}
