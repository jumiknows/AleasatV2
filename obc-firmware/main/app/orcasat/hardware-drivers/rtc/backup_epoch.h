/**
 * @file backup_epoch.h
 * @brief The backup epoch powered by a timer inside the TMS570.
 * @author Victor L
 * @date Feb 10, 2020
 */

#ifndef BACKUP_EPOCH_H_
#define BACKUP_EPOCH_H_

#include "obc_time.h"
#include "obc_error.h"

void backup_epoch_init(void);
epoch_t get_backup_epoch(void);
void set_backup_epoch(real_time_t* desired_time);

#endif // BACKUP_EPOCH_H_
