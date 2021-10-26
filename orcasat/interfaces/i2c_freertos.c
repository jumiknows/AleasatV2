/*******************************************************
 * i2c_freertos.c
 *
 * Created on: Mar 10, 2019
 * Author(s): Alireza.A + Melvin.M
 *
 *******************************************************/

#include "i2c_freertos.h"
#include "obc_task_info.h"
#include "obc_i2c.h"
#include "FreeRTOS.h"
#include "rtos_queue.h"
#include "rtos_task.h"
#include "obc_i2c.h"
#include "obc_misra.h"
#include "obc_rtos.h"
#include "obc_watchdog.h"
#include "logger.h"

/**
 * @brief  Max number of bytes for I2C write operations.
 * Value is based on the assumption that ADCS CubeComputer commands represent the largest
 * operations on the I2C bus.
 */
#define MAX_I2C_WRITE_DATA_SIZE (464 + 1) // Max CubeComputer paylod + Command ID

/**
 * @brief The time to wait for space on the command queue to free up.
 */
#define I2C_CMD_QUEUE_FREE_SPACE_WAIT_MS 100

/**
 * @brief Task buffers.
 */
StaticTask_t i2c_task_buffer;
StackType_t i2c_task_stack[I2C_TASK_STACK_SIZE];

// >> MODIFY_HERE
#define COMMAND_QUEUE_SIZE (0 + EXPANDER_RECEIVE_QUEUE_SIZE + TEMP_SENSOR_RECEIVE_QUEUE_SIZE + ADCS_RECEIVE_QUEUE_SIZE + IMU_RECEIVE_QUEUE_SIZE) // + EXAMPLE_RECEIVE_QUEUE_SIZE

// Private Functions
void notify_sender(i2c_command_t* command, result_t error);
void vI2CWorkerTask(void* pvParameters);

// >> MODIFY_HERE
// i2c_command_t example_queue_storage_buffer[EXAMPLE_RECEIVE_QUEUE_SIZE];
// StaticQueue_t example_queue_buffer;
i2c_command_t expander_queue_storage_buffer[EXPANDER_RECEIVE_QUEUE_SIZE];
StaticQueue_t expander_queue_buffer;
QueueHandle_t expander_rx_q;

i2c_command_t temp_sensor_queue_storage_buffer[TEMP_SENSOR_RECEIVE_QUEUE_SIZE];
StaticQueue_t temp_sensor_queue_buffer;
QueueHandle_t temp_sensor_rx_q;

i2c_command_t debug_cmd_queue_storage_buffer[DEBUG_CMD_RECEIVE_QUEUE_SIZE];
StaticQueue_t debug_cmd_queue_buffer;
QueueHandle_t debug_cmd_rx_q;

i2c_command_t adcs_queue_storage_buffer[ADCS_RECEIVE_QUEUE_SIZE];
StaticQueue_t adcs_queue_buffer;
QueueHandle_t adcs_rx_q;

TaskHandle_t i2c_worker_task_handle = NULL;
QueueHandle_t command_queue         = NULL;
i2c_command_t command_queue_storage_buffer[COMMAND_QUEUE_SIZE];
StaticQueue_t command_queue_buffer;

i2c_command_t imu_queue_storage_buffer[IMU_RECEIVE_QUEUE_SIZE];
StaticQueue_t imu_queue_buffer;
QueueHandle_t imu_rx_q;


/**
 * @brief Sets up the I2C hardware.
 */
void i2c_init_irq_hw(void) {
    i2c_init();
    // TODO: interrupts
}

/**
 * @brief Starts the i2c task.
 */
void i2c_start_task(void) {
    i2c_worker_task_handle = task_create_static(&vI2CWorkerTask, "worker_task", I2C_TASK_STACK_SIZE, NULL, I2C_WORKER_DEFAULT_PRIORITY, i2c_task_stack, &i2c_task_buffer);
}

/**
 * send_command_to_i2c_worker
 *
 * Queues a command in the command queue and updates the priority of the worker task.
 *
 *  command: the command to be queued
 *  priority: priority of the task that created the command
 *
 * returns: I2C_SUCCESS when the command is well-formed and an error code otherwise.
 *
 */
i2c_err_t send_command_to_i2c_worker(const i2c_command_t* command, UBaseType_t priority) {
    if ((command->wr == WRITE_DATA) && (command->num_bytes >= MAX_I2C_WRITE_DATA_SIZE)) {
        return UNEXPECTED_NUM_BYTES;
    }

    if (xQueueSend(command_queue, command, pdMS_TO_TICKS(I2C_CMD_QUEUE_FREE_SPACE_WAIT_MS)) != pdPASS) {
        return I2C_QUEUE_FULL;
    }

    if (i2c_worker_task_handle != NULL) {
        vTaskPrioritySet(i2c_worker_task_handle, priority);
    }

    return I2C_SUCCESS;
}

/**
 * @brief Creates queues required for I2C communication.
 *
 * Creates the command-queue and all the receive-queues.
 */
void i2c_create_infra(void) {
    command_queue = xQueueCreateStatic(COMMAND_QUEUE_SIZE, sizeof(i2c_command_t), (uint8_t*)&command_queue_storage_buffer, &command_queue_buffer);

    // >> MODIFY_HERE
    // example_receive_queue = xQueueCreateStatic(EXAMPLE_RECEIVE_QUEUE_SIZE, sizeof(result_t),
    // (uint8_t*) &example_queue_storage_buffer, &example_queue_buffer);
    expander_rx_q    = xQueueCreateStatic(EXPANDER_RECEIVE_QUEUE_SIZE, sizeof(result_t), (uint8_t*)&expander_queue_storage_buffer, &expander_queue_buffer);
    temp_sensor_rx_q = xQueueCreateStatic(TEMP_SENSOR_RECEIVE_QUEUE_SIZE, sizeof(result_t), (uint8_t*)&temp_sensor_queue_storage_buffer, &temp_sensor_queue_buffer);
    debug_cmd_rx_q   = xQueueCreateStatic(DEBUG_CMD_RECEIVE_QUEUE_SIZE, sizeof(result_t), (uint8_t*)&debug_cmd_queue_storage_buffer, &debug_cmd_queue_buffer);
    adcs_rx_q        = xQueueCreateStatic(ADCS_RECEIVE_QUEUE_SIZE, sizeof(result_t), (uint8_t*)&adcs_queue_storage_buffer, &adcs_queue_buffer);
    imu_rx_q         = xQueueCreateStatic(IMU_RECEIVE_QUEUE_SIZE, sizeof(result_t), (uint8_t*)&imu_queue_storage_buffer, &imu_queue_buffer);
}

/* I2C Worker Task
 * 	Processes command queue submissions.
 *
 * 	void* pvParameters: NULL
 */
void vI2CWorkerTask(void* pvParameters) {
    i2c_command_t command;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);
    i2c_err_t errcode    = I2C_SUCCESS;
    static uint8_t write_data_buffer[MAX_I2C_WRITE_DATA_SIZE];

    for (;;) {
        // TODO: make this an atomic operation
        if (uxQueueSpacesAvailable(command_queue) == COMMAND_QUEUE_SIZE) {
            vTaskPrioritySet(NULL, I2C_WORKER_DEFAULT_PRIORITY);
        }
        set_task_status(wd_task_id, task_asleep);
        xQueueReceive(command_queue, (void*)&command, portMAX_DELAY); // Wait for a command
        set_task_status(wd_task_id, task_alive);

        if (command.wr == WRITE_DATA) {
            // Ensure that command.num_bytes + 1-byte command ID will fit in write_data_buffer[]
            if (command.num_bytes < MAX_I2C_WRITE_DATA_SIZE) {
                write_data_buffer[0] = command.cmd;

                uint16_t i = 0;
                for (i = 0; i < command.num_bytes; ++i) {
                    write_data_buffer[i + 1] = command.data[i];
                }
                errcode = i2c_write(command.destination, command.num_bytes + 1, write_data_buffer);
            } else {
                errcode = CONTRACT_VIOLATION_UNEXPECTED_NUM_BYTES;
            }
        }
        /* RA: waiving MISRA checking due to CCS bug. False positive 12.2 warning */
        OBC_MISRA_CHECK_OFF
        else if (command.wr == READ_DATA) {
            //printf(" command send is %d \n", command.cmd);
            errcode = i2c_read(command.destination, command.cmd, command.num_bytes, command.data);
            //printf(" command response is %d \n", *(command.data));
        }
        OBC_MISRA_CHECK_ON

        if (command.wr == SCAN) {
            errcode = i2c_ping_device(command.destination);
        }

        notify_sender(&command, errcode);
    }
}

/**
 * notify_sender
 *
 * A helper function to write the result of the execution of a command to the receive queue
 * associated with the task that sent the command.
 *
 *  command:    the command that was dequeued from the command queue
 *  error:      the result of execution of the command
 *
 */
void notify_sender(i2c_command_t* command, result_t error) {
    // Only send the result to a receive queue if the queue was successfully created (its handle is
    // not null) in the `init` function.

    // >> MODIFY_HERE
    // if (example_receive_queue && (command->receive_queue_id == EXAMPLE_RECEIVE_QUEUE_ID)) {
    //     xQueueSend(example_receive_queue, (void*)&error, portMAX_DELAY);
    // }
    if (expander_rx_q && (command->receive_queue_id == EXPANDER_RECEIVE_QUEUE_ID)) {
        xQueueSend(expander_rx_q, (void*)&error, portMAX_DELAY);
    }
    if (temp_sensor_rx_q && (command->receive_queue_id == TEMP_SENSOR_RECEIVE_QUEUE_ID)) {
        xQueueSend(temp_sensor_rx_q, (void*)&error, portMAX_DELAY);
    }
    if (debug_cmd_rx_q && (command->receive_queue_id == DEBUG_CMD_RECEIVE_QUEUE_ID)) {
        xQueueSend(debug_cmd_rx_q, (void*)&error, portMAX_DELAY);
    }
    if (adcs_rx_q && (command->receive_queue_id == ADCS_RECEIVE_QUEUE_ID)) {
        xQueueSend(adcs_rx_q, (void*)&error, portMAX_DELAY);
    }
    if (imu_rx_q && (command->receive_queue_id == IMU_RECEIVE_QUEUE_ID)) {
       xQueueSend(imu_rx_q, (void*)&error, portMAX_DELAY);
    }

}
