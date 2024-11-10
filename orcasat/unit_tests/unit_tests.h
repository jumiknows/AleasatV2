/*
 * unit_tests.h
 *
 *  Created on: Jan 29, 2018
 *      Author: Richard, Steven
 */

#ifndef UNIT_TESTS_H_
#define UNIT_TESTS_H_
#include "sys_common.h"
#include "gio.h"
#include "FreeRTOS.h"
#include "rtos_task.h"

// Flash test
uint32_t test_flash(void);

// ECC flash test
uint32_t test_ecc_flash(void);

// MRAM test
void test_mram(void);

// AES Tests
int32_t test_aes_main(void);
int32_t test_encrypt_cbc(void);
int32_t test_decrypt_cbc(void);
int32_t test_encrypt_ctr(void);
int32_t test_decrypt_ctr(void);
int32_t test_encrypt_ecb(void);
int32_t test_decrypt_ecb(void);
void test_encrypt_ecb_verbose(void);

// Logging system test
extern TaskHandle_t xLogSysTaskHandle;
void vLogSysTestTask(void* pvParameters);

// Scheduler Test
void init_scheduler_test(void);

// State Handler Test
void init_state_handler_test(void);

// Filesystem Test
uint32_t test_filesystem(void);

// NVCT Test
void init_nvct_test(void);

#endif /* UNIT_TESTS_H_ */
