/*
 * @file obc_mram.c
 * @brief Common driver to support hardware and mock MRAMs.
 * @author Richard Arthurs
 * @date October 18, 2020
 */

#include "obc_hardwaredefs.h"
#include "obc_mram.h"

#ifdef PLATFORM_LAUNCHPAD_1224
#include "mram_mock.h"
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
#include "mram_mock.h"
#endif

/**
 * @brief Initializes the MRAM.
 * @pre MIBSPI is initialized.
 * @return whether or not the initialization was successful.
 */
mram_err_t init_mram(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return init_mock_mram();
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    return init_mock_mram();
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
mram_err_t write_mram(uint16_t addr, uint16_t size_bytes, const uint8_t *data) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return write_mock_mram(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    return write_mock_mram(addr, size_bytes, data);
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
mram_err_t read_mram(uint16_t addr, uint16_t size_bytes, uint8_t *data) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return read_mock_mram(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    return read_mock_mram(addr, size_bytes, data);
#endif
}

/**
 * @brief Resets the MRAM by power cycling it if possible.
 */
void reset_mram(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
// Not applicable!
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    // Not applicable!
#endif
}

/**
 * @brief Returns the size of the MRAM.
 * @return The size (bytes) of the MRAM.
 */
uint32_t mram_size(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return MOCK_MRAM_SIZE_BYTES;
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    return MOCK_MRAM_SIZE_BYTES;
#endif
}

/**
 * @brief Returns the base MRAM address that the application image can use.
 * The bootloader uses addresses lower than this.
 * @return The lowest MRAM address that application code may use.
 */
uint32_t application_image_base_mram_address(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return 0;
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    return 0;
#endif
}
