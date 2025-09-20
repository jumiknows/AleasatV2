/**
 * @file log_sys_serial.h
 * @brief Log system serial driver
 */

#ifndef LOG_SYS_SERIAL_H_
#define LOG_SYS_SERIAL_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void log_sys_serial_create_infra(void);
void log_sys_serial_create_task(void);

void log_sys_serial_write_log(const uint8_t *log_data, uint8_t data_len, uint32_t timeout_ticks);

#endif // LOG_SYS_SERIAL_H_
