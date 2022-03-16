/*******************************************************
 * logger.h
 *
 * Created on: May 11, 2019
 *  Author(s): Alireza.A, Julian M
 *
 *******************************************************/

#ifndef LOGGER_H_
#define LOGGER_H_

#include "FreeRTOS.h"
#include "log_ids.h"
#include "sys_common.h"

#define MAX_PAYLOAD_SIZE 245 // bytes

typedef enum { DEBUG, WARNING, INFO, ERROR } log_level_t;

typedef uint16_t log_identifier_t;
typedef uint8_t log_payload_size_t;

extern uint32_t num_logged_debug;
extern uint32_t num_logged_info;
extern uint32_t num_logged_error;
extern uint32_t num_logged_warning;

void log_data(log_level_t lvl, log_identifier_t func_id, bool write_to_file, log_payload_size_t size, void* data);
void log_str(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, ...);
void log_str_no_time(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, ...);

#endif // LOGGER_H_
