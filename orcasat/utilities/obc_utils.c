/*	obc_utils
 * 		- assorted utility functions
 */
#include <string.h>
#include <stdlib.h>
#include "obc_utils.h"
#include "obc_hardwaredefs.h"
#include "gio.h"
#include "reg_system.h"

/* busy_wait
 * 	- simply loops for as long as you want doing nothing
 */
void busy_wait(uint32_t ticks_to_wait) {
    uint32_t i = 0;
    while (i <= ticks_to_wait) {
        i++;
    }
}

/* itoa2
 * 	- int32_t to ASCII
 * 	- remember that a 32-bit signed integer requires 11 ascii chars + 1 null
 * 	- http://code.geeksforgeeks.org/lDrTiv
 */
char* itoa2(int32_t num, char* buffer, int32_t base, int32_t itr) {
    char* bsbuff;
    int32_t num2;

    num2 = num;
    if (num < 0) {
        *(buffer++) = '-';
        num         = -num;
    }

    if (num > (base - 1)) {
        bsbuff = itoa2(num / base, (buffer + 1), base, itr + 1);
    }

    if (num <= (base - 1)) {
        bsbuff        = buffer;
        *(buffer + 1) = '\0';
    }

    *(bsbuff - itr) = ((num % (base)) < 10) ? ((num % base) + 48) : ((num % base) + 55);

    return itr > 0 ? bsbuff : num2 < 0 ? buffer - 1 : buffer;
}

/* utoa2
 * 	- uint32_t to ASCII
 * 	- remember that a 32-bit integer requires 10 ascii chars + 1 null
 */
char* utoa2(uint32_t num, char* buffer, int32_t base, int32_t itr) {
    char* bsbuff;
    int32_t num2;

    num2 = num;

    if (num > (base - 1)) {
        bsbuff = utoa2(num / base, (buffer + 1), base, itr + 1);
    }

    if (num <= (base - 1)) {
        bsbuff        = buffer;
        *(buffer + 1) = '\0';
    }

    *(bsbuff - itr) = ((num % (base)) < 10) ? ((num % base) + 48) : ((num % base) + 55);

    return itr > 0 ? bsbuff : num2 < 0 ? buffer - 1 : buffer;
}

/* clear_buffer
 * 	- fill a given buffer with null characters '\0'
 */
void clear_buffer(char* buf, uint32_t length) {
    memset(buf, '\0', length);
}

/*	restart_software
 * 		- immediate software reset
 */
void restart_software(void) {
    systemREG1->SYSECR = systemREG1->SYSECR | ((uint32_t)1U << 15);
}

// ----------------------------------- MATH -----------------------------------

/**
 * @brief integral power
 * @param b: base
 * @param e: exponent
 * @returns b^e
 */
uint32_t power(uint32_t b, uint32_t e) {
    uint32_t res = 1;
    while (e > 0) {
        res *= b;
        e -= 1;
    }
    return res;
}

/**
 * @brief converts an string to an unsigned integer
 * @param seq: char sequence representing an integer MSB -> LSB
 * @returns equivalent integer
 */
uint32_t cseq_to_num(char* seq) {
    uint32_t res = 0;
    size_t len   = strlen(seq);

    uint32_t idx = 0, digit = 0;
    while (idx < len) {
        // https://stackoverflow.com/a/5030086
        digit = (uint32_t)(seq[idx] - '0');
        res += (digit * power(10, len - idx - 1));
        idx += 1;
    }

    return res;
}
