/**
 * @file telem_exec.c
 * @brief Telem executor
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "telem_exec.h"
#include "telem_gen.h"
#include "telem_error.h"
#include "telem.h"

// OBC
#include "obc_rtc.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"

// FreeRTOS
#include "rtos.h"

// Logger
#include "logger.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TELEM_QUEUE_LENGTH     100U
#define TELEM_QUEUE_ITEM_SIZE  sizeof(uint16_t)

#define TELEM_EXEC_POLL_PERIOD_MS 2000U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void telem_exec_task(void *pvParameters);

static void get_num_units(uint16_t *num_units_0, uint16_t *num_units_1, uint16_t *num_units_2);
static void write_outer_headers_and_stop_word(uint16_t num_units_0, uint16_t num_units_1, uint16_t num_units_2);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static QueueHandle_t telem_queue = NULL;
static uint8_t telem_queue_storage[TELEM_QUEUE_LENGTH * TELEM_QUEUE_ITEM_SIZE] = { 0 };
static StaticQueue_t telem_queue_buf = { 0 };

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the telem executor
 */
void telem_exec_pre_init(void) {
    telem_queue = xQueueCreateStatic(TELEM_QUEUE_LENGTH, TELEM_QUEUE_ITEM_SIZE, telem_queue_storage, &telem_queue_buf);
    obc_rtos_create_task(OBC_TASK_ID_TELEM_EXEC, &telem_exec_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Added a telem unit to the queue for execution
 *
 * @param[in] telem_id_t    ID of the telem unit to be executed
*
 * @return Status code:
 *            - TELEM_SUCCESS if the command was added to the queue successfully
 *            - TELEM_ERR_ENQUEUE_FAIL if a timeout occurred waiting for space in the queue
 */
void telem_exec_enqueue(const uint16_t telem_unit_id) {
    // Queue should be available immediately so timeout is 0
    if (xQueueSend(telem_queue, &telem_unit_id, 0) != pdTRUE) {
        LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_ENQUEUE_FAIL);
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void telem_exec_task(void *pvParameters) {
    TickType_t telem_exec_poll_period_ticks = pdMS_TO_TICKS(TELEM_EXEC_POLL_PERIOD_MS);
    uint16_t telem_unit_id;

    vPortTaskUsesFPU();

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_TELEM_EXEC);

        if (ulTaskNotifyTake(pdFALSE, telem_exec_poll_period_ticks)) {
            while (xQueueReceive(telem_queue, &telem_unit_id, 0) == pdTRUE) {

                if (telem_unit_id == TELEM_QUEUE_MAGIC_NUM) {

                    uint16_t num_units_0 = 0;
                    uint16_t num_units_1 = 0;
                    uint16_t num_units_2 = 0;
                    get_num_units(&num_units_0, &num_units_1, &num_units_2);
                    write_outer_headers_and_stop_word(num_units_0, num_units_1, num_units_2);

                } else {

                    // Invoke telem unit
                    telem_err_t err = telem_invoke_unit((telem_id_t) telem_unit_id);

                    if (err != TELEM_SUCCESS) {
                        LOG_TELEM__EXECUTION_FAILURE(err);
                    }
                }
            }
        }
    }
}

/*
 * @brief Get the number of units from queue after seeing a magic num
*/
static void get_num_units(uint16_t *num_units_0, uint16_t *num_units_1, uint16_t *num_units_2) {
    if (xQueueReceive(telem_queue, num_units_0, 0) != pdTRUE) {
        LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_QUEUE_MAGIC_NUM);
    }

    if (xQueueReceive(telem_queue, num_units_1, 0) != pdTRUE) {
        LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_QUEUE_MAGIC_NUM);
    }

    if (xQueueReceive(telem_queue, num_units_2, 0) != pdTRUE) {
        LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_QUEUE_MAGIC_NUM);
    }
}

/*
 * @brief Add the outer headers to write queue
*/
static void write_outer_headers_and_stop_word(uint16_t num_units_0, uint16_t num_units_1, uint16_t num_units_2) {
    if (num_units_0) {
        if (write_outer_header_and_stop_word(num_units_0, "telem_priority_0", 0) != TELEM_SUCCESS) {
            LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_GEN_HEADER);
        }
    }

    if (num_units_1) {
        if (write_outer_header_and_stop_word(num_units_1, "telem_priority_1", 1) != TELEM_SUCCESS) {
            LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_GEN_HEADER);
        }
    }

    if (num_units_2) {
        if (write_outer_header_and_stop_word(num_units_2, "telem_priority_2", 2) != TELEM_SUCCESS) {
            LOG_TELEM__EXECUTION_FAILURE(TELEM_ERR_GEN_HEADER);
        }
    }
}
