/**
 * @file flash_mock.h
 * @brief
 * @author Andrada Zoltan
 * @date March 26, 2019
 */

#ifndef FLASH_MOCK_H_
#define FLASH_MOCK_H_

#include "sys_common.h"
#include "obc_flash.h"
#include "obc_error.h"

void flash_init_mock(void);
flash_err_t flash_erase_mock(uint32_t addr, flash_erase_sz_t erase_size);
flash_err_t flash_write_mock(uint32_t addr, uint32_t size_bytes, const uint8_t* data);
flash_err_t flash_read_mock(uint32_t addr, uint32_t size_bytes, uint8_t* data);

#endif /* FLASH_MOCK_H_ */
