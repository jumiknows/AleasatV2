/**
 * @file obc_featuredefs.h
 * @brief Feature definitions for launchpad platform.
 * @author Richard Arthurs
 * @date Nov 4, 2018
 *
 * The top level featuredefs file that enables one platform-specific featuredefs file per build
 * configuration.
 *
 * Please include this in your code if you require featuredefs functionality.
 */

#ifndef OBC_FEATUREDEFS_H_
#define OBC_FEATUREDEFS_H_

/******************************************************************************
 *
 * SETTINGS FOR LAUNCHPAD-1224
 *  - TMS570LS1224-equipped launchpads acquired in 2018
 *
 *****************************************************************************/
#ifdef PLATFORM_LAUNCHPAD_1224
#include "launchpad_1224_featuredefs.h"
#endif

/******************************************************************************
 *
 * SETTINGS FOR PLATFORM-ALEA-V1
 *  - initial ALEA hardware
 *
 *****************************************************************************/
#ifdef PLATFORM_ALEA_V1
#include "alea_v1_featuredefs.h"
#endif /* PLATFORM_ALEA_V1 */

#endif /* OBC_FEATUREDEFS_H_ */
