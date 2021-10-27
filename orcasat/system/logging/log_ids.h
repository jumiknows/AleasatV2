/* 
 * log_ids.h
 *
 *  Created on: Jun 19, 2019
 *  Re-generated: 02:56 AM on October 27, 2021
 * 
 *      Author: Richard
 *      
 *      IDs for logging system
 *      
 *      NOTE: this is an automatically generated file from the OBC system sheet, please do not update it. 
*/

// Turn off formatting so that the template doesn't get adjusted by the formatter.
// clang-format off

#ifndef LOG_ID_H_
#define LOG_ID_H_


#define PRINT_GENERAL 1 /* General purpose printing */

#define HW_TYPE 2 /* Hardware type at startup */

#define START_TYPE 3 /* Startup type (reason for reset) */

#define PBIST_STATUS 4 /* PBIST status */

#define SCHED_LOG 5 /* Scheduler messages */

#define ACK_CMD 6 /* Ack command */

#define HELP_CMD 7 /* Help command */

#define CMD_GENERAL 8 /* Command system general */

#define TEMP_MUTEX 9 /* Temperature mutex */

#define ADC_ID 10 /* ADC */

#define TASK_OVERFLOW 11 /* Task overflow */

#define SUBCMD 12 /* Subcommand processing */

#define GET_HEAP_CMD 13 /* Get heap command */

#define GET_RUNTIME_CMD 14 /* Get runtime command */

#define GET_MINHEAP_CMD 15 /* Get minheap command */

#define GET_TASKS_CMD 16 /* Get tasks command */

#define I2C_LOG 17 /* I2c */

#define PING_CMD 18 /* Ping command for detection by Houston */

#define UNEXPECTED_PAYLOAD_LEN 19 /* Unexpected payload size error  */

#define UNEXPECTED_MSG_ID 20 /* Unexpected message ID */

#define TEMP_LOG 21 /* Temperature sensor */

#define FLASH_LOG 22 /* Flash memory tests */

#define GET_EPOCH_CMD 23 /* Get epoch command */

#define RAMOCCUR_LOG 24 /* ECC register telemetry */

#define RTC_A_LOG 25 /* RTC A */

#define RTC_B_LOG 26 /* RTC B */

#define RTC_REG_SCAN 27 /* RTC register scan */

#define RTC_LOG 28 /* RTC General */

#define CMD_ERR_OPCODE 29 /* Command Invalid */

#define CMD_SAFE_INVALID 30 /* Previously-scheduled command safe mode error */

#define CMD_RX 31 /* Command reception */

#define GPIO_EXPAND_LOG 32 /* GPIO Expander */

#define GPIO_LOG 33 /* GPIO */

#define SCHEDULER_FAILED_TO_READ_RTC_CURRENT_TIME 34 /* Scheduler Failed to read the current time from RTC */

#define GET_BACKUP_EPOCH_CMD 35 /* Get backup epoch command */

#define STATE_LOG 36 /* State handler */

#define CMD_ERR_PARSE 37 /* Failed to parse command */

#define FS_LOG 38 /* Filesystem log messages */

#define WD_LOG 39 /* Watchdog */

#define SYS_LOG 40 /* System log */

#define TELEM_INFRA 41 /* Telemetry logging infrastructure */

#define OBC_TASK 42 /* OBC RTOS task control */

#define BOARD_ID 43 /* Platform type ID */

#define FILE_DOWNLINK 44 /* Used during file downlink */

#define GPS_LOG 45 /* GPS module */

#define HANG_TASK_LOG 46 /* Hang task */

#define ECHO_CMD 47 /* Echo command */

#define SEQ_NUM_CMD 48 /* Get sequence number command */

#define ADCS_LOG 49 /* ADCS log */

#define SETTINGS_LOG 50 /* Settings */

#define ACTION_ID_EXCEEDED 51 /* Action ID scheduler tried to assign was larger than max allowed action ID */

#define TEST_ACK_CMD 52 /* ACK Test Command */

#define TEST_COMMS_CMD 53 /* Comms Test */

#define TEST_CAN_GPIO_CMD 54 /* CAN GPIO Test */

#define PWR_FLT_LOG 55 /* Power Fault Log */

#define TEST_PCMD_GATING_CMD 56 /* PCMD Power Gating Test */

#define COMMS_OPENLST 57 /* COMMS Openlst */

#define COMMS_ACK_CMD 58 /* COMMS ACK */

#define ADCS_IMU_LOG 59 /* ADCS IMU Log */

#define ADCS_GYRO_LOG 60 /* ADCS GYRO */


// clang-format on

#endif /* LOG_ID_H_ */
