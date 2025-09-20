/*
 * @file obc_mram.c
 * @brief Common driver to support hardware and mock MRAMs.
 */

#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_mram.h"

#if FEATURE_HW_MRAM
#include "mr25h256.h"
#else
#include "mram_mock.h"
#endif

/**
 * @brief Initializes the MRAM.
 * @pre MIBSPI is initialized.
 * @return whether or not the initialization was successful.
 */
mram_err_t mram_init(void) {
#if FEATURE_HW_MRAM
    return mram_mr25h256_init();
#else
    return mram_mock_init();
#endif
}

/**
 * @brief Writes to MRAM.
 *
 * @param[in] addr the Address to begin writing at.
 * @param[in] size_bytes The number of bytes to write.
 * @param[in] data The data to write to MRAM. Must be at least @ref size_bytes in size.
 * @return MRAM_OK or an error code.
 */
mram_err_t mram_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data) {
#if FEATURE_HW_MRAM
    return mram_mr25h256_write(addr, size_bytes, data);
#else
    return mram_mock_write(addr, size_bytes, data);
#endif
}

/**
 * @brief Reads data from MRAM.
 *
 * @param[in] addr The address at which to begin reading.
 * @param[in] size_bytes How many bytes to read.
 * @param[out] data Data buffer where the data will be placed. Valid if @c MRAM_OK is returned.
 * @return MRAM_OK or an error code.
 */
mram_err_t mram_read(uint16_t addr, uint16_t size_bytes, uint8_t *data) {
#if FEATURE_HW_MRAM
    return mram_mr25h256_read(addr, size_bytes, data);
#else
    return mram_mock_read(addr, size_bytes, data);
#endif
}

/**
 * @brief Returns the size of the MRAM.
 * @return The size (bytes) of the MRAM.
 */
uint32_t mram_size(void) {
#if FEATURE_HW_MRAM
    return MR25H256_MRAM_MAX_ADDRESS;
#else
    return MOCK_MRAM_SIZE_BYTES;
#endif
}

/**
 * @brief Returns the base MRAM address that the application image can use.
 * The bootloader uses addresses lower than this.
 * @return The lowest MRAM address that application code may use.
 */
uint32_t application_image_base_mram_address(void) {
#if FEATURE_HW_MRAM
    return 0;
#else
    return 0;
#endif
}
