/**
 * @file obc_task_info.h
 * @brief Definitions of task priorities and stack sizes.
 * @date April 29, 2020
 * @author ORCASat C&DH Team
 */

#ifndef OBC_TASK_INFO_H_
#define OBC_TASK_INFO_H_

/**
 * @brief Task stack sizes. These sizes are in units of 32 bits (4 bytes).
 */
#define MAIN_TASK_STACK_SIZE            (1000)
#define UART_TASK_STACK_SIZE            (1500)
#define GPIO_IRQ_TASK_STACK_SIZE        (400)
#define BLINKY_TASK_STACK_SIZE          (128)
#define EXPANDER_BLINKY_TASK_STACK_SIZE (400)
#define I2C_TASK_STACK_SIZE             (800)
#define NVCT_SYNC_STACK_SIZE            (128)
#define TELEM_TASK_STACK_SIZE           (500)
#define FS_MASTER_TASK_STACK_SIZE       (5096)
#define FS_LIFECYCLE_TASK_STACK_SIZE    (512)
#define BLOCK_ERASE_TASK_STACK_SIZE     (128)
#define SCHEDULER_TASK_STACK_SIZE       (640)
#define SYNC_TASK_STACK_SIZE            (128)
#define SORT_TASK_STACK_SIZE            (128)
#define WDPT_TASK_STACK_SIZE            (256)
#define STATE_HANDLER_STACK_DEPTH       (128)
#define HANG_TASK_STACK_SIZE            (128)
#define COMMS_IRQ_TASK_STACK_SIZE       (400)

/**
 * @brief Maximum priority that any task may have.
 */
#define MAX_PRIORITY (configMAX_PRIORITIES - 1)

/**
 * @brief The executer task stack size in units of 32 bits (4 bytes).
 *
 * This task runs the scheduled functions. Therefore, it must have a stack size large enough to run
 * the most demanding action. If this task tries to run an action that requires a larger stack than
 * is available, the task stack overflow hook will be called, which will issue a logger message
 * indicating that the executer task has overflowed, or it will hang.
 */
#define EXECUTER_TASK_STACK_SIZE (1300)

/**
 * @brief Task default priorities.
 */
#define MAIN_TASK_PRIORITY               2
#define BLINKY_TASK_DEFAULT_PRIORITY     3
#define SCHEDULER_TEST_TASK_PRIORITY     4
#define STATE_HANDLER_TEST_TASK_PRIORITY 4
#define SERIAL_TASK_DEFAULT_PRIORITY     5
#define WATCHDOG_TASK_DEFAULT_PRIORITY   1
#define STATE_HANDLER_TASK_PRIORITY      5
#define HANG_TASK_DEFAULT_PRIORITY       1

/**
 * @brief I2C FreeRTOS priorities.
 */
#define I2C_WORKER_DEFAULT_PRIORITY 5

/**
 * @brief GPIO interrupt handling priority.
 */
#define GPIO_IRQ_SERVICE_PRIORITY 4

/**
 * @brief Comms interrupt handling priority.
 */
#define COMMS_IRQ_SERVICE_PRIORITY 3

/**
 * @brief Priorities for the scheduler.
 */
#define SCHEDULER_TASK_PRIORITY configMAX_PRIORITIES - 2
#define EXECUTER_TASK_PRIORITY  configMAX_PRIORITIES - 3
#define SYNC_TASK_PRIORITY      configMAX_PRIORITIES - 4
#define SORT_TASK_PRIORITY      configMAX_PRIORITIES - 4

/**
 * @brief Telemetry logger priority.
 */
#define TELEM_LOG_TASK_PRIORITY 4

/**
 * @brief Filesystem priorities.
 */
#define FS_LIFECYCLE_PRIORITY 3
#define FS_MASTER_PRIORITY    3
#define BLOCK_ERASE_PRIORITY  1

#endif /* OBC_TASK_INFO_H_ */
