/*	@file obc_utils
 *  @brief assorted utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_utils.h"
#include "obc_hardwaredefs.h"
#include "gio.h"
#include "reg_system.h"

// Standard library
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief simply loops for as long as you want doing nothing
 *
 * @param[in] ticks_to_wait: number of ticks to loop for
 */
void busy_wait(uint32_t ticks_to_wait) {
    uint32_t i = 0;
    while (i <= ticks_to_wait) {
        i++;
    }
}

/**
 * @brief Generic delay function with 1us/tick resolution
 *
 * @param us Number of us to delay
 * @note The maximum allowed delay time for this function is 537 seconds
*/
void obc_delay_us(uint32_t us)
{

    uint32_t dt = 0;             // units of 0.125 us;
    uint32_t init_time = rtiREG1->CNT[0U].FRCx;

    /**
     * RTI counter 0 has base tick rate of 125ns, therefore there are 8 rti ticks per us
    */

    while ((dt/8) < us) {
        dt = (rtiREG1->CNT[0U].FRCx - init_time);
    }

}


/**
 * @brief int32_t to ASCII
 *
 * - remember that a 32-bit signed integer requires 11 ascii chars + 1 null
 * - http://code.geeksforgeeks.org/lDrTiv
 *
 * @param[in] num: number to be converted
 * @param[in] buffer: empty space to use
 * @param[in] base: base for converting
 * @param[in] itr: accumulator (starts at 0)
 * @return ASCII equivalent of the integer
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

/**
 * @brief uint32_t to ASCII
 *
 * remember that a 32-bit signed integer requires 10 ascii chars + 1 null
 *
 * @param[in] num: number to be converted
 * @param[in] buffer: empty space to use
 * @param[in] base: base for converting
 * @param[in] itr: accumulator (starts at 0)
 * @return ASCII equivalent of the unsigned integer
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

/**
 * @brief fill a given buffer with null characters '\0'
 *
 * @param[in] buf: buffer to be filled
 * @param[in] length: number of bytes to set to
 */
void clear_buffer(char* buf, uint32_t length) {
    memset(buf, '\0', length);
}

/**
 * @brief immediate software reset
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
 * @brief converts a string to an unsigned integer and checks for error
 *
 * @param[in] seq: char sequence representing an integer MSB -> LSB
 * @return equivalent integer or -1 if there is an error
 */
int32_t cseq_to_num(char* seq) {
    char* str = NULL;
    uint32_t res = strtoul(seq, &str, 10);

    if(seq == str) {
        return -1;
    }
    else {
        return res;
    }
}
