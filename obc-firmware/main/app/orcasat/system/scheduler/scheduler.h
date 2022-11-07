/*******************************************************
 * scheduler.h
 *
 * Created on: May 22, 2019
 *  Author(s): Alireza.A
 *
 *******************************************************/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "rtos.h"
#include "obc_rtc.h"
#include "obc_error.h"

/**
 * @brief Possible submission sources for scheduler actions.
 * INTERNAL: OBC scheduling actions for itself.
 * EXTERNAL: from a command.
 *
 * @warning Update @ref submission_source_strings if changing this enum.
 * @warning This enum must start at 0 and monotonically increase so that
 * it can be used to index @ref submission_source_strings.
 */
typedef enum { INTERNAL = 0, EXTERNAL = 1 } scheduler_submission_source_t;

typedef void (*scheduler_action_t)(uint32_t arg_len, void* arg);

typedef uint32_t action_id_t;

// Startup steps
void scheduler_create_infra(void);
scheduler_err_t scheduler_start(void);

// API
scheduler_err_t scheduler_schedule_action_start_now(action_id_t* id, scheduler_submission_source_t source, uint32_t interval, scheduler_action_t action, uint8_t argument_size, void* argument);

scheduler_err_t scheduler_schedule_action(action_id_t* id, scheduler_submission_source_t source, real_time_t start_at, uint32_t interval, scheduler_action_t action, uint8_t argument_size,
                                          void* argument);

void scheduler_invoke_action(scheduler_action_t action, uint8_t argument_size, void* argument);

scheduler_err_t scheduler_remove_action(action_id_t id);
scheduler_err_t scheduler_get_period(action_id_t id, uint32_t* period);
scheduler_err_t scheduler_set_period(action_id_t id, uint32_t period);

void print_schedule(void);

#endif // SCHEDULER_H_
