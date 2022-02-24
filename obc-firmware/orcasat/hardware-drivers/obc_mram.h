/*
 * @file obc_mram.h
 * @brief Common driver to delegate to mock MRAM or real MRAM drivers.
 * @author Richard Arthurs
 * @date October 18, 2020
 */

#ifndef OBC_MRAM_H_
#define OBC_MRAM_H_

#include "obc_error.h"
#include "sys_common.h"

mram_err_t init_mram(void);
mram_err_t write_mram(uint16_t addr, uint16_t size_bytes, const uint8_t* data);
mram_err_t read_mram(uint16_t addr, uint16_t size_bytes, uint8_t* data);
void reset_mram(void);

uint32_t mram_size(void);
uint32_t application_image_base_mram_address(void);

#endif /* OBC_MRAM_H_ */
