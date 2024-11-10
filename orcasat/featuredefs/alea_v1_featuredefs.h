/**
 * @file alea_v1_featuredefs.h
 * @brief Feature definitions for ALEASAT OBC v1 hardware platform.
 * @author Noah Tajwar
 * @date February 5, 2021
 */

#ifndef ALEA_V1_FEATUREDEFS_H_
#define ALEA_V1_FEATUREDEFS_H_
#ifdef PLATFORM_ALEA_V1

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
#define RTC_MOCK_EN 1 // TODO switch to hardware RTC (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/58)

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
#define GPIO_EXPANDER_EN 1

/**
 * @brief RTI and RTOS have vPortPreemptiveTick.
 * In order to not have multiple declarations, RTI vPortPreemptiveTick needs to be commented out
 *
 * 1: Uncomments the RTI's vPortPreemptiveTick
 * 0: USE THIS! Comments out the RTI's vPortPreemptiveTick
 *
 */
#define USE_RTI_PREEMPTIVE_TICK 0

/**
 * @brief The message to be used for reply of board type through the command system
 */
#define BOARD_TYPE_MSG "ALEASAT OBC v1"

#endif /* PLATFORM_ALEA_V1 */
#endif /* ALEA_V1_FEATUREDEFS_H_ */
