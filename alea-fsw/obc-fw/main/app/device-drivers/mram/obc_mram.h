/*
 * @file obc_mram.h
 * @brief Common driver to delegate to mock MRAM or real MRAM drivers.
 */

#ifndef OBC_MRAM_H_
#define OBC_MRAM_H_

#define MRAM_RESET_COUNTER_ADDR  0x1000

/**
 * @brief Return value for mram functions, indicating any errors.
 */
typedef enum mram_err {
    /**
     * @brief Indicates that there were no IO errors.
     */
    MRAM_OK = 0,

    /**
     * @brief Indicates there was a request to access MRAM that does not exist.
     */
    MRAM_INDEX_OUT_OF_BOUND = -1,

    /**
     * @brief Indicates there was a low-level error in the MIBSPI driver.
     */
    MRAM_MIBSPI_ERR = -2,

    /**
     * @brief Indicates there was an error attempting to grab the MIBSPI mutex.
     */
    MRAM_MIBSPI_MUTEX_GRAB_ERR = -3
} mram_err_t;



mram_err_t mram_init(void);
mram_err_t mram_write(uint16_t addr, uint16_t size_bytes, const uint8_t *data);
mram_err_t mram_read(uint16_t addr, uint16_t size_bytes, uint8_t *data);
void mram_reset(void);

uint32_t mram_size(void);
uint32_t application_image_base_mram_address(void);

#endif /* OBC_MRAM_H_ */
