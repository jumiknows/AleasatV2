/**
 * @file test_commands.h
 * @brief Test commands not to be used in actual flight software
 */

#ifndef TEST_COMMANDS_H_
#define TEST_COMMANDS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// General
void cmd_test_ack(uint32_t arg_len, void* arg);

// Comms
void cmd_test_comms_raw(uint32_t arg_len, void* arg);
void cmd_test_comms_tx_only(uint32_t arg_len, void* arg);
void cmd_test_comms_tx_rx(uint32_t arg_len, void* arg);
void cmd_test_comms_stress1(uint32_t arg_len, void* arg);
void cmd_test_comms_flash_app(uint32_t arg_len, void* arg);
void cmd_test_comms_reboot(uint32_t arg_len, void* arg);
void cmd_test_comms_get_telem(uint32_t arg_len, void* arg);

// Magnetorquer test commands
void cmd_test_mag(uint32_t arg_len, void* arg);
void cmd_test_mag_all(uint32_t arg_len, void* arg);
void cmd_test_mag_init(uint32_t arg_len, void* arg);
void cmd_test_panel_gyro(uint32_t arg_len, void* arg);

// IMU
void cmd_imu_test(uint32_t arg_len, void* arg);

// CAN GPIO
void cmd_test_can_gpio(uint32_t arg_len, void* arg);

// EPS
void cmd_test_eps_read_float(uint32_t arg_len, void* arg);
void cmd_test_eps_read_float_raw(uint32_t arg_len, void* arg);
void cmd_test_eps_read_int(uint32_t arg_len, void* arg);
void cmd_test_eps_write(uint32_t arg_len, void* arg);
void cmd_test_eps_read_sanity(uint32_t arg_len, void* arg);
void cmd_test_eps_writeread_sanity(uint32_t arg_len, void* arg);

// FLASH
void cmd_test_flash_rw(uint32_t arg_len, void* arg);

// FILESYSTEM
void cmd_test_fs(uint32_t arg_len, void* arg);

#endif // TEST_COMMANDS_H_
