/**
 * @file backup_epoch.h
 * @brief The backup epoch powered by a timer inside the TMS570.
 */

#ifndef BACKUP_EPOCH_H_
#define BACKUP_EPOCH_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_time.h"
#include "obc_error.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void backup_epoch_init(void);
epoch_t get_backup_epoch(void);
epoch_t get_uptime(void);
void set_backup_epoch(real_time_t *desired_time);

#endif // BACKUP_EPOCH_H_
