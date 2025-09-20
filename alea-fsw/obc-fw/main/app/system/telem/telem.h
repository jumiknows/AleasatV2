/**
 * @file telem.h
 * @brief Telemetry
 */

#ifndef TELEM_H_
#define TELEM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "obc_time.h"
#include "telem_error.h"
#include "telem_gen.h"
#include "data_fmt.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TELEM_QUEUE_MAGIC_NUM   65535U
#define TELEM_STOP_WORD         0xA1EA50BC // "ALEA OBC"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void telem_init_mram(void);
void telem_collect_post_init(void);
void telem_collect_enable(void);
void telem_collect_disable(void);

// getters/setters for config data, only use these for getting the priority/period
telem_err_t telem_get_priority(telem_id_t id, uint8_t *output);
telem_err_t telem_get_period(telem_id_t id, uint32_t *output);
telem_err_t telem_set_priority(telem_id_t id, uint8_t input);
telem_err_t telem_set_period(telem_id_t id, uint32_t input);

telem_err_t telem_handle_resp(const telem_id_t id, const uint8_t priority, const void *resp_struct, const data_fmt_desc_t *resp_desc,
                              const uint16_t resp_len, uint8_t *buf);

telem_err_t telem_get_last_value(const telem_id_t telem_id, uint8_t *buf, epoch_t *exec_epoch, uint16_t mutex_timeout);

telem_err_t write_outer_header_and_stop_word(uint16_t num_units, char *filename, uint8_t priority);

/*
 * @brief Converts the priority number (0,1,2) into the appropriate filename
 */
static inline void convert_priority_to_filename(char *filename, uint8_t priority) {
    switch (priority) {
    case 0:
        strcpy(filename, "telem_priority_0");
        break;

    case 1:
        strcpy(filename, "telem_priority_1");
        break;

    default: // Also doubles as case 2
        strcpy(filename, "telem_priority_2");
    }
}

#endif // TELEM_H_
