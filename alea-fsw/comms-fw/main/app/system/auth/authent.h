#ifndef _AUTHENT_H
#define _AUTHENT_H

#include <stdint.h>
#include <stdbool.h>

#include "aes.h"
#include "radio.h"

// Subtract this from the current time when checking timestamp to account for our time maybe being slightly fast
#define TIMESTAMP_POSITIVE_OFFSET_SECONDS 5
// Amount of time the timestamp can be behind by. It can be ahead by any amount.
#define TIMESTAMP_VALID_WINDOW_SECONDS (TIMESTAMP_POSITIVE_OFFSET_SECONDS + 30)

extern __xdata uint8_t aro_key[AES_KEY_SIZE];
extern __xdata uint8_t authent_enable;

__bit check_packet_authent(__xdata rf_buffer_t* pkt);
__bit add_packet_authent(__xdata rf_buffer_t* pkt);

#endif /* _AUTHENT_H */
