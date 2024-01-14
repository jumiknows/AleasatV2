/**
 * @file obc_hardwaredefs.h
 * @brief Top-level hardware definitions file that enables linking the platform-specific file.
 * @author Richard Arthurs
 * @date Oct 4, 2018
 *
 * HARDWARE DEFINITIONS
 *
 * To facilitate easy switching of peripherals and pins, all hardware connections will be defined as
 * macros here. That way, we just update the actual GPIO pin here, and the macro assigned to say,
 * the RTC CS pin (GPIO) will be updated to the correct one.
 *
 * This file uses build configuration-defined symbols to choose a single hardwaredefs file to
 * include. These files include any hardware-specific renaming or hard-coded config parameters that
 * are required for a platform's code to build.
 */

#ifndef OBC_HWDEFS_H_
#define OBC_HWDEFS_H_

/******************************************************************************
 *
 * SETTINGS FOR LAUNCHPAD-1224
 *  - TMS570LS1224-equipped launchpads acquired in 2018
 *
 *****************************************************************************/
#ifdef PLATFORM_LAUNCHPAD_1224
#include "launchpad_1224_hardwaredefs.h"
#endif

/******************************************************************************
 *
 * SETTINGS FOR PLATFORM-ALEA-V1
 *  - initial ALEA hardware
 *
 *****************************************************************************/
#ifdef PLATFORM_ALEA_V1
#include "alea_v1_hardwaredefs.h"
#endif /* PLATFORM_ORCA_V1 */

#endif /* OBC_HWDEFS_H_ */
