/**
 * @file obc_filesystem.h
 * @brief OBC filesystem, handles all filesystem related activities
 */

#ifndef OBC_FILESYSTEM_H_
#define OBC_FILESYSTEM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_error.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

fs_err_t fs_init(void);
fs_err_t fs_deinit(void);

fs_err_t fs_self_test(void);

#endif /* OBC_FILESYSTEM_H_ */
