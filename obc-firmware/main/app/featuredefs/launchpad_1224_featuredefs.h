/**
 * @file launchpad_1224_featuredefs.h
 * @brief Feature definitions for TMS570LS1224 LaunchPad.
 * @author Melvin Matthews
 * @date November 04, 2018
 *
 *  A set of #defines to enable and disable features. This is intended to allow configurability
 *  in cases where hardware features are damaged or unavailable, such as when operating with
 *  just a LaunchPad.
 */

#ifndef LAUNCHPAD_1224_FEATUREDEFS_H_
#define LAUNCHPAD_1224_FEATUREDEFS_H_
#ifdef PLATFORM_LAUNCHPAD_1224

/**
 * @brief OBC IDLE SLEEP ENABLE
 *
 * 	1: OBC will enter a sleep mode during the RTOS idle task
 * 	0: OBC will not enter a sleep mode during the RTOS idle task
 */
#define OBC_IDLE_SLEEP_EN 1

/**
 * @brief GIO INTERRUPT EXAMPLE ENABLE
 *
 * 1: GIO interrupt example will run.
 * 0: GIO interrupt example will not run.
 */
#define GIO_EXAMPLE_EN 0

/**
 * @brief RTC MOCK ENABLE
 *
 * 1: RTC mock implementation is used.
 * 0: Real hardware driver is used for RTC.
 */
#define RTC_MOCK_EN 1

/**
 * @brief TEMP SENSOR EXAMPLE ENABLE
 *
 * 1: temp sensor example is enabled (runs periodically)
 * 0: the temp sensor example is disabled
 */
#define TEMP_SENSOR_EXAMPLE_EN 1

/**
 * @brief GPIO EXPANDER ENABLE
 *
 * 1: GPIO expander is present on the hardware
 * 0: no GPIO expander is present and GPIO expander functions will not be used.
 */
#define FEATURE_GPIO_EXPANDER 0

#define FEATURE_HEARTBEAT 1

/**
 * @brief IMU 2 ENABLE
 *
 * 1: IMU 2 is present on the hardware
 * 0: IMU 2 is not present on the hardware and cannot be used
 */
#define IMU_2_EN 0

/**
 * @brief RTI and RTOS have vPortPreemptiveTick.
 * In order to not have multiple declarations, RTI vPortPreemptiveTick needs to be commented out
 *
 * 1: Uncomments the RTI's vPortPreemptiveTick
 * 0: USE THIS! Comments out the RTI's vPortPreemptiveTick
 */
#define USE_RTI_PREEMPTIVE_TICK 0

/**
 * @brief Include C source file of Comms test app image bytes in build.
 * C file is large and only needed for specific purpose.
 * If not included, cmd_test_comms_flash_app command will fail.
 *
 * 1: Include test app image bytes
 * 0: Don't include test app image bytes
 *
 * @ref comms_app_image.c
 *
 */
#define INCLUDE_COMMS_FIRMWARE_TEST_IMAGE 0

/**
 * @brief Enable if hardware watchdog is present and must be pet periodically
 */
#define FEATURE_HW_WATCHDOG 0

#endif /* PLATFORM_LAUNCHPAD_1224 */
#endif /* LAUNCHPAD_1224_FEATUREDEFS_H_ */
