/**
 * @file alea_v1_hardwaredefs.h
 * @brief Hardware definitions for ALEASAT OBC v1 platform.
 *
 * PURPOSE
 *   - Hardwaredefs files are used to map hardware peripheral names to common names used throughout our code
 *   - This allows different peripherals to be used on different boards, with the same codebase
 *   - These files also allow hardware features to be disabled. This is useful if no RTC is connected, for example
 *
 * INSTRUCTIONS
 *   - When developing software that requires a peripheral register pointer (such as GIOPORTA), define your
 *     own descriptive name for it (BLINKY_GIO_PORT), and use that in your code.
 *        - Use a #define in the appropriate hwdefs file to map BLINKY_GIO_PORT to GIOPORTA
 *        - Assuming the hardware (GIO port A, in this case) is configured in HALCoGen, the mapping should work.
 *        - Once verified, add the mapping to other platforms' hwdefs, and configure it for them in HALCoGEN.
 */

#ifndef ALEA_V1_HARDWAREDEFS_H_
#define ALEA_V1_HARDWAREDEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_gpio.h"
#include "obc_featuredefs.h"
#include "tms_can.h"

// HAL
#include "system.h"
#include "reg_rti.h"
#include "reg_sci.h"
#include "reg_i2c.h"
#include "reg_gio.h"
#include "reg_het.h"
#include "reg_spi.h"
#include "reg_mibspi.h"
#include "reg_etpwm.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief OBC hardware type indicator.
 *
 * This is a master indicator that the platform is any version of an OBC (as opposed to a LaunchPad).
 * All OBC platforms must define this value, regardless of their revision number.
 */
#if defined(PLATFORM_ALEA_V1)
#define PLATFORM_OBC 1
#endif

/**
 * @brief General functionality: peripheral names and RTOS configuration.
 */
#define RTI_CLK_SPEED_HZ ((uint32_t)RTI_FREQ * 1000000) // For RTOS tick
#define UART_DEBUG       sciREG                         // Debug/development UART, 115200 baud
#define UART_GPS         scilinREG                      // GPS UART, 115200 baud
#define I2C              i2cREG1

/**
 * @brief OBC Blinky LED port and pin
 */
#define	OBC_BLINKY_PORT GIO_PORT(gioPORTA)
#define OBC_BLINKY_PIN  7

// =============================================================================
// GPIO Expander
// =============================================================================

/**
 * @brief GPIO expander chip configuration.
 *
 * Configuration for TMS570 peripherals required for interaction with the GPIO expander.
 * The IRQ pin is asserted by the expander when one of its channels detects an interrupt. It is an input to the TMS570.
 */
#define EXPANDER_ADDR         0x20
#define OBC_EXPAND_RESET_PORT hetPORT1 // Active-low reset for GPIO expander.
#define OBC_EXPAND_RESET_PIN  26
#define OBC_EXPAND_IRQ_N_PORT gioPORTB // Active-low IRQ output from GPIO expander.
#define OBC_EXPAND_IRQ_N_PIN  0

/**
 * @brief GPIO expander pin configuration.
 *
 * Configurations for GPIO expander inputs/output pins, not pins on the TMS570 that interact with the expander.
 */
#define EXPANDER_BLINKY_PORT EXP_PORT(GPIO_EXP_PORT_1) // Expander output connected to blinky LED.
#define EXPANDER_BLINKY_PIN  7

// =============================================================================
// Watchdog
// =============================================================================

/**
 * @brief Watchdog pins and configuration.
 */
#define OBC_WD_PORT      hetPORT2
#define OBC_WD_PIN       0
#define WD_PET_PERIOD_MS 20000 // Watchdog pet period should be between WDL_MAX_MS and WDU_MIN_MS
#define WD_PET_PULSE_MS  2     // Minimum pulse is 50 ns

/**
 * @brief Hardware-Dependent Watchdog Timing Values
 *
 * Calculated for watchdog capacitor with C = 1uF
 */
#define WD_COEFF   0.125
#define WDU_MIN_MS 65836
#define WDU_TYP_MS 77455
#define WDU_MAX_MS 89073
#define WDL_MIN_MS (WD_COEFF * WDU_MIN_MS)
#define WDL_TYP_MS (WD_COEFF * WDU_TYP_MS)
#define WDL_MAX_MS (WD_COEFF * WDU_MAX_MS)

// =============================================================================
// MIBSPI / SPI
// =============================================================================

/**
 * @brief Pin numbers for when MIBSPI pins are used as GIOs
 */
#define MIBSPI_PIN_CS(_x)   (_x)
#define MIBSPI_PIN_ENA      8
#define MIBSPI_PIN_CLK      9
#define MIBSPI_PIN_SIMO(_x) ((_x) + 16)
#define MIBSPI_PIN_SOMI(_x) ((_x) + 24)

/**
 * @brief OBC SPI
 *
 * Includes solar panel sensors and RTC
 */
#define OBC_SPI_PORT spiREG4

/**
 * @brief Solar panel pin configuration.
 */
#define SOLAR_CS_1A_PORT mibspiPORT1
#define SOLAR_CS_1A_PIN  MIBSPI_PIN_CS(2)
#define SOLAR_CS_1B_PORT mibspiPORT1
#define SOLAR_CS_1B_PIN  MIBSPI_PIN_CS(4)
#define SOLAR_CS_2A_PORT mibspiPORT1
#define SOLAR_CS_2A_PIN  MIBSPI_PIN_CS(5)
#define SOLAR_CS_2B_PORT hetPORT1
#define SOLAR_CS_2B_PIN  23
#define SOLAR_CS_3A_PORT gioPORTB
#define SOLAR_CS_3A_PIN  2
#define SOLAR_CS_3B_PORT hetPORT1
#define SOLAR_CS_3B_PIN  9
#define SOLAR_CS_4A_PORT hetPORT1
#define SOLAR_CS_4A_PIN  12
#define SOLAR_CS_4B_PORT hetPORT1
#define SOLAR_CS_4B_PIN  30

#define SOLAR_PWR_EN_PORT CAN_PORT(canREG3)
#define SOLAR_PWR_EN_PIN  CAN_PIN_TX

#define GYRO_PWR_EN_PORT  hetPORT1
#define GYRO_PWR_EN_PIN   16

/**
 * @brief RTC pin configuration.
 *
 * TODO: Hardware RTC is currently disabled in alea_v1_featuredefs.h since firmware needs to be adapted for only 1 RTC
 * (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/58)
 */
#define RTCA_CS_PORT    spiPORT4
#define RTCA_CS_PIN     MIBSPI_PIN_CS(0)
#define RTCA_TS_PORT    hetPORT1
#define RTCA_TS_PIN     28
#define RTCA_ALARM_PORT gioPORTB
#define RTCA_ALARM_PIN  3

/**
 * @brief RTC MIBSPI configuration.
 *
 * The RTC uses two transfer groups:
 *     one 8-byte group for time setting/getting
 *     one 2-byte group for generic register accesses.
 *
 * TODO: Reconfigure (see above)
 * (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/58)
 */
#define RTC_TIME_MIBSPI_GROUP 0
#define RTC_REG_MIBSPI_GROUP  1
#define RTC_TIME_MIBSPI_REG   OBC_MIBSPI
#define RTC_REG_MIBSPI_REG    OBC_MIBSPI
#define RTC_DATA_FORMAT       2

/**
 * @brief Flash MIBSPI configuration.
 */
#define FLASH_MIBSPI_REG    mibspiREG5
#define FLASH_DATA_FORMAT   0
#define FLASH_1_BYTE_GROUP  0 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP  1 // transfer group with 2 byte length
#define FLASH_5_BYTE_GROUP  2 // transfer group with 5 byte length
#define FLASH_20_BYTE_GROUP 3 // transfer group with 20 byte length
#define FLASH_69_BYTE_GROUP 4 // transfer group with 69 byte length

#define FLASH_CS0_PORT mibspiPORT5
#define FLASH_CS0_PIN  MIBSPI_PIN_ENA

#define FLASH_RESET_PORT GIO_PORT(gioPORTA)
#define FLASH_RESET_PIN  1

/*
 * @brief Flash size configuration
 * @brief Flash size configuration - 32MB = 256Mb
 */
#define EXT_FLASH_BLOCK_SIZE  4096
#define EXT_FLASH_BLOCK_COUNT 8192

/**
 * @brief COMMS SPI interface
 */
#define COMMS_MIBSPI_REG       mibspiREG3
#define COMMS_CS_PORT          gioPORTA
#define COMMS_CS_PIN           5
#define COMMS_TRANSFER_GROUP   0
#define COMMS_TG_SIZE_WORDS    128 // one word is 16 bits
#define COMMS_DATA_FORMAT      0
#define COMMS_MIBSPI_TGI_LEVEL 0

#define COMMS_RST_PORT CAN_PORT(canREG1)
#define COMMS_RST_PIN  CAN_PIN_RX

#define COMMS_WD_PORT  CAN_PORT(canREG1)
#define COMM_WD_PIN    CAN_PIN_TX

#define COMMS_INT_REG  hetREG1
#define COMMS_INT_PORT hetPORT1
#define COMMS_INT_PIN  31
#define COMMS_INT_EDGE 0

#define COMMS_PWR_5V0_FLT_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define COMMS_PWR_5V0_FLT_PIN  1
#define COMMS_PWR_3V3_FLT_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define COMMS_PWR_3V3_FLT_PIN  2

/**
 * @brief Camera
 */
#define CAMERA_MIBSPI_REG     mibspiREG1
#define CAMERA_CS_PORT        mibspiPORT1
#define CAMERA_CS_PIN         0
#define CAMERA_RX_INT_LEVEL   0
#define CAMERA_TG_SINGLE      0
#define CAMERA_TG_BURST       1
#define CAMERA_TG_BURST_WORDS 127 // one word is 16 bits

#define CAMERA_PWR_EN_PORT  GIO_PORT(hetPORT1)
#define CAMERA_PWR_EN_PIN   1
#define CAMERA_PWR_FLT_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define CAMERA_PWR_FLT_PIN  5

// =============================================================================
// I2C
// =============================================================================

/**
 * @brief IMU
 */
#define IMU_PWR_EN_PORT GIO_PORT(hetPORT1)
#define IMU_PWR_EM_PIN  8

// =============================================================================
// GPS
// =============================================================================

/**
 * @brief GPS pin configuration.
 */
#define GPS_RST_PORT   CAN_PORT(canREG3)
#define GPS_RST_PIN    CAN_PIN_RX
#define GPS_P1PPS_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define GPS_P1PPS_PIN  3
#define GPS_STS_PORT   EXP_PORT(GPIO_EXP_PORT_0)
#define GPS_STS_PIN    4

#define GPS_PWR_EN_PORT  GIO_PORT(gioPORTB)
#define GPS_PWR_EN_PIN   1
#define GPS_PWR_FLT_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define GPS_PWR_FLT_PIN  0

// =============================================================================
// PWM
// =============================================================================

/**
 * @brief Magnetorquers
 */
#define MAG1A_PWM_REG   pwm1 //HET_1[30]
#define MAG1B_PWM_REG   pwm2 //HET_1[12]
#define MAG1_SLEEP_PIN  10 //HET_1[10]

#define MAG2A_PWM_REG   pwm3 //HET_1[18]
#define MAG2B_PWM_REG   pwm4 //HET_1[20]
#define MAG2_SLEEP_PIN  17 //HET_1[17]

#define MAG3A_PWM_REG   pwm5 //HET_1[4]
#define MAG3B_PWM_REG   pwm6 //HET_1[7]
#define MAG3_SLEEP_PIN  22 //HET_1[22]

#define MAG_PWR_EN_PORT GIO_PORT(hetPORT1)
#define MAG_PWR_EN_PIN  14 //HET_1[14]

#define MAG_HET hetRAM1
#define MAG_SLEEP_PORT GIO_PORT(hetPORT1)

/**
 * @brief Centrifuge
 */
#define RXN_PWM_REG      etpwmREG1 // output B
#define RXN_BRAKE_PORT   GIO_PORT(hetPORT1)
#define RXN_BRAKE_PIN    25
#define RX_FREQ_FB_PORT  GIO_PORT(mibspiPORT5)
#define RX_FREQ_FB_PIN   MIBSPI_PIN_CS(0)
#define RXN_LOCK_PORT    EXP_PORT(GPIO_EXP_PORT_1)
#define RXN_LOCK_PIN     1
#define RXN_DIR_PORT     EXP_PORT(GPIO_EXP_PORT_1)
#define RXN_DIR_PIN      0

#define RXN_HALLA_PORT   EXP_PORT(GPIO_EXP_PORT_1)
#define RXN_HALLA_PIN    6
#define RXN_HALLB_PORT   EXP_PORT(GPIO_EXP_PORT_1)
#define RXN_HALLB_PIN    5
#define RXN_HALLC_PORT   EXP_PORT(GPIO_EXP_PORT_1)
#define RXN_HALLC_PIN    3

#define RXN_PWR_EN_PORT  GIO_PORT(hetPORT1)
#define RXN_PWR_EN_PIN   11
#define RXN_PWR_FLT_PORT EXP_PORT(GPIO_EXP_PORT_0)
#define RXN_PWR_FLT_PIN  6

// =============================================================================
// SPI
// =============================================================================
/**
 * @brief Solar panel gyro SPI
 *
*/
#define PANEL0_CS_PORT GIO_PORT(hetPORT1)
#define PANEL1_CS_PORT GIO_PORT(hetPORT1)
#define PANEL2_CS_PORT GIO_PORT(gioPORTB)
#define PANEL3_CS_PORT CAN_PORT(canREG2)

#define PANEL0_CS_PIN 19
#define PANEL1_CS_PIN 24
#define PANEL2_CS_PIN 2
#define PANEL3_CS_PIN CAN_PIN_RX //CAN_PIN_RX

#define PANEL0_SPI_PORT spiREG4
#define PANEL1_SPI_PORT spiREG4
#define PANEL2_SPI_PORT spiREG4
#define PANEL3_SPI_PORT spiREG4

/*****************************************************************************/
/* ADC                                                                       */
/*****************************************************************************/

/*
 * @brief Defines corresponding channel numbers on ADC group 1
 */
#define ADC_CHANNEL_VBAT_MONITOR       23
#define ADC_CHANNEL_COMMS_PWR_IMON_3V3 22
#define ADC_CHANNEL_COMMS_PWR_IMON_5V0 21
#define ADC_CHANNEL_PAY_PWR_IMON       20
#define ADC_CHANNEL_ACS_GPS_PWR_IMON   19
#define ADC_CHANNEL_RXN_PWR_IMON       18
#define ADC_CHANNEL_OBC_PWR_IMON       17
#define ADC_CHANNEL_SOLAR_4_SUN        3
#define ADC_CHANNEL_SOLAR_3_SUN        2
#define ADC_CHANNEL_SOLAR_2_SUN        1
#define ADC_CHANNEL_SOLAR_1_SUN        0

// =============================================================================
// RTI
// =============================================================================

/**
 * @brief Configuration for backup epoch.
 *
 * The backup epoch uses the RTI module counter block 1 and compare register 1
 * to generate an interrupt once per second.
 *
 * The counter block configured to feed compare reg 1 is configured in
 * HALCoGEN.
 *
 * The compare register period is set to 1000 ms.
 */

#define BACKUP_EPOCH_RTI_REG           rtiREG1
#define BACKUP_EPOCH_RTI_COMPARE_REG   1
#define BACKUP_EPOCH_RTI_COUNTER_BLOCK rtiCOUNTER_BLOCK1

/**
 * @brief IMU enable interface
*/
#define IMU_1_EN_PIN 8
#define IMU_1_EN_PORT GIO_PORT(hetPORT1)
#define IMU_2_EN_PIN 7
#define IMU_2_EN_PORT EXP_PORT(GPIO_EXP_PORT_0)

#endif /* ALEA_V1_HARDWAREDEFS_H_ */
