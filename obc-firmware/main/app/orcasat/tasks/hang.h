/**
 * @file hang_timeout.c
 * @brief Tasks that hangs for testing purposes.
 * @author Victor Leon
 * @date May 19, 2020
 */

#ifndef ORCASAT_TASKS_HANG_H_
#define ORCASAT_TASKS_HANG_H_

void hang_start_task(void);
bool set_hang_task_status_alive(void);

#endif /* ORCASAT_TASKS_HANG_H_ */
