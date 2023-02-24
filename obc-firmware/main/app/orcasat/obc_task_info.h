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
#define CMD_SYS_EXEC_TASK_STACK_SIZE    (1024)
#define CMD_SYS_IMM_TASK_STACK_SIZE     (256)
#define CMD_SYS_SCHED_TASK_STACK_SIZE   (256)
#define SERIAL_RX_TASK_STACK_SIZE       (256)
#define SERIAL_TX_TASK_STACK_SIZE       (256)
#define GPIO_IRQ_TASK_STACK_SIZE        (400)
#define BLINKY_TASK_STACK_SIZE          (128)
#define EXPANDER_BLINKY_TASK_STACK_SIZE (400)
#define I2C_TASK_STACK_SIZE             (800)
#define NVCT_SYNC_STACK_SIZE            (128)
#define TELEM_TASK_STACK_SIZE           (500)
#define FS_LOGGER_TASK_STACK_SIZE       (500)
#define SCHEDULER_TASK_STACK_SIZE       (640)
#define SYNC_TASK_STACK_SIZE            (128)
#define SORT_TASK_STACK_SIZE            (128)
#define WDPT_TASK_STACK_SIZE            (256)
#define STATE_HANDLER_STACK_DEPTH       (128)
#define HANG_TASK_STACK_SIZE            (128)
#define COMMS_IRQ_TASK_STACK_SIZE       (400)
#define RTC_MOCK_STACK_SIZE             (128)

/**
 * @brief Maximum priority that any task may have.
 */
#define MAX_PRIORITY (configMAX_PRIORITIES - 1)

/**
 * @brief Task default priorities.
 */
#define MAIN_TASK_PRIORITY               2
#define BLINKY_TASK_DEFAULT_PRIORITY     3
#define SCHEDULER_TEST_TASK_PRIORITY     4
#define STATE_HANDLER_TEST_TASK_PRIORITY 4
#define SERIAL_RX_TASK_PRIORITY          5
#define SERIAL_TX_COMMS_TASK_PRIORITY    3
#define SERIAL_TX_LOGS_TASK_PRIORITY     2
#define CMD_SYS_IMM_TASK_PRIORITY        4
#define CMD_SYS_SCHED_TASK_PRIORITY      4
#define CMD_SYS_EXEC_TASK_PRIORITY       4
#define WATCHDOG_TASK_DEFAULT_PRIORITY   1
#define STATE_HANDLER_TASK_PRIORITY      5
#define HANG_TASK_DEFAULT_PRIORITY       1
#define RTC_MOCK_TASK_PRIORITY           5

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
 * @brief Telemetry logger priority.
 */
#define TELEM_LOG_TASK_PRIORITY 4

/**
 * @brief Filesystem priorities.
 */
#define FS_LOGGER_PRIORITY    3

#endif /* OBC_TASK_INFO_H_ */
