/* @file test_commands.h
 * @date Mar 24, 2021
 * @author Julian Mentasti
 */

#ifndef TEST_COMMANDS_H_
#define TEST_COMMANDS_H_

void cmd_test_ack(uint32_t arg_len, void* arg);
void cmd_test_comms_spi(uint32_t arg_len, void* arg);


// Magnetorquer test commands
void cmd_test_mag(uint32_t arg_len, void* arg);
void cmd_test_mag_all(uint32_t arg_len, void* arg);
void cmd_test_mag_init(uint32_t arg_len, void* arg);

/*IMU test loop*/
void cmd_imu_test(uint32_t arg_len, void* arg);

#endif /* TEST_COMMANDS_H_ */
