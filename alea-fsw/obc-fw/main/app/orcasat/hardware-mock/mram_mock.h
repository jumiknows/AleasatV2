#ifndef ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H
#define ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H

#include "rtos.h"
#include "obc_error.h"

#define MOCK_MRAM_SIZE_BYTES 400U // number of addressable bytes

mram_err_t init_mock_mram(void);
mram_err_t write_mock_mram(uint32_t addr, uint32_t size, const uint8_t *data);
mram_err_t read_mock_mram(uint32_t addr, uint32_t size, void *data);

#endif // ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H
