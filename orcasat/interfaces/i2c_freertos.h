/*******************************************************
 * i2c_freertos.h
 *
 * Created on: Mar 10, 2019
 * Author(s): Alireza.A + Melvin.M
 *
 * Each client task is associated with a receive-queue from where it receives
 * the result of the execution of the commands it has send to the command-queue.
 *
 * When sending a command the client task specifies the queue it wants to receive the result
 * from later (here EXAMPLE_RECEIVE_QUEUE_ID).
 * It can then block on the receive-queue (here example_recieve_queue) it has specified
 * waiting for the result of execution.
 * A client should not send more commands than there is free space in it receive queue.
 *
 * To work with the command queue, you can follow the example below and update any where in the
 * i2c_freertos.h and i2c_freertos.c files where the key word `>> MODIFY_HERE` is added.
 *
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 * #include "i2c_freertos.h"
 *
 * #define EXAMPLE_TASK_PRIORITY 4
 * #define SOME_STACK_SIZE 300
 *
 *  void client_example_task(void* arg) {
 *      result_t result;
 *	    uint8_t success_count = 0;
 *	    uint8_t failure_count = 0;
 *      uint8_t destination = 0, cmd = 0; // target destination and register
 *	    uint8_t data = 0;
 *
 *      i2c_command_t command = {cmd, EXAMPLE_RECEIVE_QUEUE_ID, destination, WRITE_DATA, 1, &data};
 *
 *      for (;;) {
 *	        send_command_to_i2c_worker(command, EXAMPLE_TASK_PRIORITY);
 *
 *		    xQueueReceive(example_receive_queue, (void *)&result, portMAX_DELAY);
 *
 *		    if (result != SUCCESS) {
 *			    success_count += 1;
 *		    } else {
 *			    failure_count += 1;
 *		    }
 *
 *		    vTaskDelay(10000 / portTICK_RATE_MS);
 *	    }
 *  }
 *
 *  void app_main() {
 *      // create the worker task
 *      xTaskCreate(vI2CWorkerTask, "worker_task", SOME_STACK_SIZE, NULL,
 *I2C_WORKER_DEFAULT_PRIORITY, &i2c_worker_task_handle);
 *      // initialize the queues
 *      i2c_err_t errcode = init_i2c_command_and_receive_queues();
 *      if (errcode != SUCCESS)
 *          return;
 *      // create the worker and then clients
 *      xTaskCreate(client_example_task, "example_task", SOME_STACK_SIZE, NULL,
 *EXAMPLE_TASK_PRIORITY, NULL);
 *      // ...
 *  }
 *
 *******************************************************/

#ifndef _I2C_FREERTOS_H
#define _I2C_FREERTOS_H

#include "FreeRTOS.h"
#include "rtos_queue.h"
#include "rtos_task.h"
#include "obc_error.h"

// >> MODIFY_HERE
// #define EXAMPLE_RECEIVE_QUEUE_ID 1
// #define EXAMPLE_RECEIVE_QUEUE_SIZE 1
#define EXPANDER_RECEIVE_QUEUE_ID   2
#define EXPANDER_RECEIVE_QUEUE_SIZE 2

#define TEMP_SENSOR_RECEIVE_QUEUE_ID   3
#define TEMP_SENSOR_RECEIVE_QUEUE_SIZE 1

#define DEBUG_CMD_RECEIVE_QUEUE_ID   4
#define DEBUG_CMD_RECEIVE_QUEUE_SIZE 1

#define IMU_RECEIVE_QUEUE_ID    5
#define IMU_RECEIVE_QUEUE_SIZE  1

#define ADCS_RECEIVE_QUEUE_ID   4
#define ADCS_RECEIVE_QUEUE_SIZE 2

// >> MODIFY_HERE
// extern QueueHandle_t example_receive_queue;
extern QueueHandle_t temp_sensor_rx_q;
extern QueueHandle_t expander_rx_q;
extern QueueHandle_t debug_cmd_rx_q;
extern QueueHandle_t adcs_rx_q;
extern QueueHandle_t imu_rx_q;

extern TaskHandle_t i2c_worker_task_handle;
extern QueueHandle_t command_queue;

typedef enum { READ_DATA, WRITE_DATA, SCAN } i2c_cmd_t;

typedef i2c_err_t result_t; // the result of execution of a command passed to the command queue

typedef struct {
    uint8_t cmd;              // Sent to the slave to select a register/memory
    uint8_t receive_queue_id; // Where the I2C Task sends the result of executing the command
    uint8_t destination;      // The slave address
    i2c_cmd_t wr;             // Whether we want to read or write data
    uint16_t num_bytes;       // Size of the data buffer in bytes
    uint8_t* data;            // A pointer to the actual data
} i2c_command_t;

// Startup steps
void i2c_create_infra(void);
void i2c_init_irq_hw(void);
void i2c_start_task(void);

// API
i2c_err_t send_command_to_i2c_worker(const i2c_command_t* command, UBaseType_t priority);

#endif /* #ifndef _I2C_FREERTOS_H */
