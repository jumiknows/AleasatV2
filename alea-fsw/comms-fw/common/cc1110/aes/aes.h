#ifndef _AES_H
#define _AES_H

#include <stdint.h>

/**
 * @brief Keys lengths must be 16 bytes or 128 bits for the AES coprocessor
 */
#define AES_KEY_SIZE   16

/**
 * @brief Block size is 16 bytes or 128 bits
 */
#define AES_BLOCK_SIZE 16

void aes_init();
void aes_compute_cbc_mac(__xdata uint8_t *data, uint16_t data_len, __xdata uint8_t *key, __xdata uint8_t *result);

#endif /* _AES_H */
