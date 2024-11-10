#include "mram_mock.h"
#include "rtos_semphr.h"

static uint8_t MRAM[MOCK_MRAM_SIZE_BYTES] = {0};

// ------------------------------------ API -----------------------------------

mram_err_t init_mock_mram(void) {
    return MRAM_OK;
}

mram_err_t write_mock_mram(uint32_t addr, uint32_t size, const uint8_t* data) {
    if ((addr + size) > MOCK_MRAM_SIZE_BYTES) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    memcpy(MRAM + addr, data, size);
    return MRAM_OK;
}

mram_err_t read_mock_mram(uint32_t addr, uint32_t size, void* data) {
    if ((addr + size) > MOCK_MRAM_SIZE_BYTES) {
        return MRAM_INDEX_OUT_OF_BOUND;
    }

    memcpy(data, MRAM + addr, size);
    return MRAM_OK;
}
