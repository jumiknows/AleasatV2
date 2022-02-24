/*
 * @file flash_mt25ql.h
 * @brief Driver for the MT25QL NOR Flash device.
 * @author Andrada Zoltan
 * @date July 24, 2019
 *
 *      In HALCoGEN, 4 transfer groups were created. This is somewhat efficient, as we don't waste
 * time sending a ton of dummy bytes when we just need to send a single byte command, as we would
 * with only 1 large transfer group.
 *
 *      TG number	Number of bytes
 *      2			1
 *      3			2
 *      4			4
 *      5			20
 *
 *      Keep in mind that ERASED flash is all 1's. So don't fill space with 0's if you will want to
 * use it later, since erasing is slow.
 */

#ifndef FLASH_MT25QL_H_
#define FLASH_MT25QL_H_

#include "obc_flash.h"

void mt25ql_init(void);
flash_err_t mt25ql_read_status_register(uint8_t* reg_data);
flash_err_t mt25ql_read_flag_status_register(uint8_t* reg_data);
flash_err_t mt25ql_write_status_register(const uint8_t* reg_data);
flash_err_t mt25ql_erase(uint32_t addr, flash_erase_sz_t erase_size);
flash_err_t mt25ql_write(uint32_t addr, uint32_t size_bytes, const uint8_t* data);
flash_err_t mt25ql_read(uint32_t addr, uint32_t size_bytes, uint8_t* data);

#endif /* FLASH_MT25QL_H_ */
