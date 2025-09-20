#ifndef ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H
#define ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H

#include "rtos.h"
#include "obc_mram.h"

#define MOCK_MRAM_SIZE_BYTES ((uint16_t) 4096U) // number of addressable bytes

mram_err_t mram_mock_init(void);
mram_err_t mram_mock_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data);
mram_err_t mram_mock_read(uint16_t addr, uint16_t size_bytes, uint8_t *data);

#endif // ORCASAT_HARDWARE_MOCK_MRAM_MOCK_H
