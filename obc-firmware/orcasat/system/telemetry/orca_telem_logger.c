/**
 * @file orca_telem_logger.c
 * @brief Telemetry logging infrastructure.
 * @author Richard A
 * @date October 31, 2019
 *
 * Telemetry logging is based on requests submitted on the telemetry logging queue. These requests
 * tell the telemetry logger task which data bin needs to be saved in flash. When the telemetry
 * logger task receives items on this queue, it grabs the appropriate data from the snapshot, and
 * performs the filesystem operations to commit this data to flash.
 */

#include "orca_telem_logger.h"
#include "orca_telemetry_defs.h"
#include "telem_snapshot.h"
#include "logger.h"
#include "rtos.h"
#include "obc_task_info.h"
#include "telem_obc.h"
#include "telem_eps.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "scheduler.h"
#include <string.h>

// Telemetry logging queue setup
#define QUEUE_LENGTH \
    (8) // Number of outstanding telemetry requests. This should be greater than the greatest number \
        // of data bins with identical collection rates.
#define TIMEOUT_MS \
    (2000) // Time to wait for the telemetry logging queue to have free space before abandoning the \
                                       // telemetry log request.

#define TELEM_FAST_RATE 30000 // Telemetry collection rate (in ms)

// Telemetry collection periods
#define TELEM_CONDN_PERIOD  2
#define TELEM_NORMAL_PERIOD 3
#define TELEM_SLOW_PERIOD   4

#define MAX_INDEX 240

#define ITEM_SIZE (sizeof(data_bin_t)) // Size of items in the telemetry logging queue
uint8_t telem_queue_storage[QUEUE_LENGTH * ITEM_SIZE];
QueueHandle_t telem_queue = NULL;

// Static logging task setup
StaticTask_t telem_collect_task_buf;
StackType_t telem_collect_stack[TELEM_TASK_STACK_SIZE];

StaticTask_t telem_task_buf;
StackType_t telem_stack[TELEM_TASK_STACK_SIZE];

// Private Function Declarations
static void log_file(data_bin_t data_bin, const char* filename, const void* data, uint32_t size);

static void vTelemCollect(void* pvParameters);
static void vTelemLoggerTask(void* pvParameters);

/**
 * @brief Creates the telemetry logging queue.
 */
void telem_create_infra(void) {
    static StaticQueue_t telem_static_queue;
    telem_queue = xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, telem_queue_storage, &telem_static_queue);
}

/**
 * @brief Starts the telemetry logging handler task.
 *
 * This task processes telemetry logging requests which pull
 * data from the snapshot and save it to files.
 */
void telem_start_task(void) {
    TaskHandle_t xTelemCollectTaskHandle = task_create_static(&vTelemCollect, "telem_collect", TELEM_TASK_STACK_SIZE, NULL, TELEM_LOG_TASK_PRIORITY, telem_collect_stack, &telem_collect_task_buf);
    TaskHandle_t xTelemTaskHandle        = task_create_static(&vTelemLoggerTask, "telem_log", TELEM_TASK_STACK_SIZE, NULL, TELEM_LOG_TASK_PRIORITY, telem_stack, &telem_task_buf);
}

/**
 * @brief Issues a telemetry logging request to the telemetry logger, notifying it that there is
 * telemetry to be logged.
 *
 * This function is called by the various telemetry logging tasks and actions when they are ready to
 * log telemetry.
 *
 * If it is not possible to log telemetry, a message will be printed and the telemetry request will
 * be dropped.
 *
 * @param bin The data bin that needs to be logged by the logger task.
 */
void log_telem(data_bin_t bin) {
    if (telem_queue != 0) {
        if (!xQueueSend(telem_queue, (void*)&bin, 0)) {
            log_str(ERROR, LOG_TELEM_INFRA, true, "Telemetry queue full.");
        }
    }
}

//-------- Private Functions -----------

static void vTelemCollect(void* pvParameters) {
    static uint32_t collectIndex = 0;
    task_id_t wd_task_id         = WD_TASK_ID(pvParameters);

    /* Initialize Tick Count Variable With the Current Time. */
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        /* Set task status to alive. */
        set_task_status(wd_task_id, task_alive);

        obc_fast_telem_collect();
        eps_fast_telem_collect();

        if ((collectIndex % TELEM_SLOW_PERIOD) == 0) {
            obc_slow_telem_collect();
            eps_slow_telem_collect();
        }

        if ((collectIndex % TELEM_CONDN_PERIOD) == 0) {
            eps_condn_telem_collect();
        }

        if ((collectIndex % TELEM_NORMAL_PERIOD) == 0) {
            eps_normal_telem_collect();
        }

        collectIndex = (++collectIndex % MAX_INDEX);

        /* Telemetry is Collected At Fast Rate. */
        set_task_status(wd_task_id, task_asleep);
        vTaskDelayUntil(&xLastWakeTime,                // Time At Which Task Was Last Unblocked
                        pdMS_TO_TICKS(TELEM_FAST_RATE) // Fast Telemetry Rate
        );
    }
}

/**
 * @brief Task that wakes up when items are pushed into the telemetry logging queue.
 *
 * This task processes the requests from the queue, which indicate which telemetry data bin should
 * be logged. It writes the telemetry data bin wrapper (such as obc_fast_telem_wrapper_t) to the
 * file, which includes the timestamp and the telemetry values.
 */
static void vTelemLoggerTask(void* pvParameters) {
    data_bin_t bin;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
        set_task_status(wd_task_id, task_asleep);

        if (telem_queue != NULL) {
            xQueueReceive(telem_queue, &bin, portMAX_DELAY);
            set_task_status(wd_task_id, task_alive);

            // Log the appropriate data based on the request type
            switch (bin) {
                case OBC_FAST_TELEM:
                    // TODO: should never need to reference file names outside of obc_filesystem
                    log_file(bin, "OBC_FAST_FILENAME", (const void*)&snapshot.obc_fast, sizeof(snapshot.obc_fast));
                    break;
                case OBC_SLOW_TELEM:
                    log_file(bin, "OBC_SLOW_FILENAME", (const void*)&snapshot.obc_slow, sizeof(snapshot.obc_slow));
                    break;
                case EPS_NORMAL_TELEM:
                    log_file(bin, "EPS_NORMAL_FILENAME", (const void*)&snapshot.eps_normal, sizeof(snapshot.eps_normal));
                    break;
                case EPS_FAST_TELEM:
                    log_file(bin, "EPS_FAST_FILENAME", (const void*)&snapshot.eps_fast, sizeof(snapshot.eps_fast));
                    break;
                case EPS_SLOW_TELEM:
                    log_file(bin, "EPS_SLOW_FILENAME", (const void*)&snapshot.eps_slow, sizeof(snapshot.eps_slow));
                    break;
                case EPS_CONDN_TELEM:
                    log_file(bin, "EPS_CONDN_FILENAME", (const void*)&snapshot.eps_condn, sizeof(snapshot.eps_condn));
                    break;
                default:
                    log_str(ERROR, LOG_TELEM_INFRA, true, "Invalid telem request bin of %d", bin);
                    break;
            }
        }
    }
}

/**
 * @brief Writes telemetry data to flash.
 *
 * The data written to the file is intended to be the telemetry data bin wrapper such as
 * obc_fast_telem_wrapper_t, although the actual data written is controlled by the caller.
 *
 * @param[in] databin The data bin that is being logged.
 * @param[in] filename The name of the file to write.
 * @param[in] data Pointer to the data to write, which will be a pointer to a data bin in the
 * telemetry snapshot.
 * @param[in] size Size (bytes) of the data to log into the file.
 */
static void log_file(data_bin_t data_bin, const char* filename, const void* data, uint32_t size) {
#if 0 // TODO fix as part of logger refactor
    if (!fs_initialized) {
        log_str(ERROR, LOG_TELEM_INFRA, false, "No FS: %s", filename);
        return;
    }

    // Assemble the file name: curr_dir/filename
    char file_path[LFS_NAME_MAX] = { 0 };
    make_file_path(filename, file_path);

    // Write the data to the file.
    fs_err_t err = fs_write((const char*)file_path, 0, (const uint8_t*)data, size, LFS_SEEK_END);
    if (err == FS_OK) {
        log_str(DEBUG, LOG_TELEM_INFRA, false, "Logged telem: %s", filename);
    } else {
        log_str(DEBUG, LOG_TELEM_INFRA, false, "%s failed", filename, err);
    }
#endif
}
