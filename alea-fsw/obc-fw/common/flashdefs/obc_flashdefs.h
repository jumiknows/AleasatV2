/**
 * @file obc_flashdefs.h
 * @brief Hardware definitions for external flash.
*/

#ifndef OBC_FLASHDEFS_H_
#define OBC_FLASHDEFS_H_

/******************************************************************************
 *
 * SETTINGS FOR LAUNCHPAD-1224
 *  - TMS570LS1224-equipped launchpads acquired in 2018
 *
 *****************************************************************************/
#ifdef PLATFORM_LAUNCHPAD_1224
#include "launchpad_1224_flashdefs.h"
#endif

/******************************************************************************
 *
 * SETTINGS FOR PLATFORM-ALEA-V1
 *  - initial ALEA hardware
 *
 *****************************************************************************/
#ifdef PLATFORM_ALEA_V1
#include "alea_v1_flashdefs.h"
#endif /* PLATFORM_ALEA_V1 */

#endif // OBC_FLASHDEFS_H_
