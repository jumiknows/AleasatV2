#include "mram_mock.h"
#include "rtos.h"

static uint8_t MRAM_MOCK_ARR[MOCK_MRAM_SIZE_BYTES] = {0};

// ------------------------------------ API -----------------------------------

mram_err_t mram_mock_init(void) {
    return MRAM_OK;
}

mram_err_t mram_mock_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data) {
    if ((addr + size_bytes) >= MOCK_MRAM_SIZE_BYTES) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    memcpy(MRAM_MOCK_ARR + addr, data, size_bytes);
    return MRAM_OK;
}

mram_err_t mram_mock_read(uint16_t addr, uint16_t size_bytes, uint8_t *data) {
    if ((addr + size_bytes) >= MOCK_MRAM_SIZE_BYTES) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    memcpy(data, MRAM_MOCK_ARR + addr, size_bytes);
    return MRAM_OK;
}
