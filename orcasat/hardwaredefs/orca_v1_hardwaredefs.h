/**
 * @file orca_v1_hardwaredefs.h
 * @brief Hardware definitions for ORCASat OBC v1 platform.
 * @author Melvin Mathews, Julie Belleville, Richard Arthurs
 * @date Jan 22, 2019
 *
 * 	PURPOSE
 *  Hardwaredefs files are used to map hardware peripheral names to common names used throughout our
 *code This allows different peripherals to be used on different boards, with the same codebase
 *  These files also allow hardware features to be disabled. This is useful if no RTC is connected,
 *for example
 *
 *	INSTRUCTIONS
 *	When developing software that requires a peripheral register pointer (such as GIOPORTA), define
 *your own descriptive name for it (BLINKY_GIO_PORT), and use that in your code.
 *		- Use a #define in the appropriate hwdefs file to map BLINKY_GIO_PORT to GIOPORTA
 *		- Assuming the hardware (GIO port A, in this case) is configured in HALCoGen, the mapping
 *should work.
 *		- Once verified, add the mapping to other platforms' hwdefs, and configure it for them in
 *HALCoGEN.
 */

#ifndef ORCA_V1_HARDWAREDEFS_H_
#define ORCA_V1_HARDWAREDEFS_H_

#include "obc_featuredefs.h"
#include "obc_gpio.h"
#include "system.h"

/**
 * @brief OBC hardware type indicator.
 *
 * This is a master indicator that the platform is any version of an OBC (as opposed to a
 * LaunchPad). All OBC platforms must define this value, regardless of their revision number.
 */
#if defined(PLATFORM_ORCA_V1)
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
#define OBC_BLINKY_PORT gioPORTA
#define OBC_BLINKY_PIN  7

/**
 * @brief GPIO expander chip configuration.
 *
 * Configuration for TMS570 peripherals required for interaction with the GPIO expander.
 * The IRQ pin is asserted by the expander when one of its channels detects an interrupt. It is an
 * input to the TMS570.
 */
#define EXPANDER_ADDR         0x20
#define OBC_EXPAND_RESET_PORT hetPORT1 // Active-low reset for GPIO expander.
#define OBC_EXPAND_RESET_PIN  16
#define OBC_EXPAND_IRQ_N_PORT gioPORTB // Active-low IRQ output from GPIO expander.
#define OBC_EXPAND_IRQ_N_PIN  0

/**
 * @brief GPIO expander pin configuration.
 *
 * Configurations for GPIO expander inputs/output pins, not pins on the TMS570 that interact with
 * the expander.
 */
#define EXPANDER_BLINKY_PORT    EXPANDER_PORT_1 // Expander output connected to blinky LED.
#define EXPANDER_BLINKY_PIN     7
#define OBC_EXPAND_IN_TEST_PORT EXPANDER_PORT_0 // Input for GPIO expander interrupt testing
#define OBC_EXPAND_IN_TEST_PIN  2

/**
 * @brief Watchdog pins and configuration.
 */
#define OBC_WD_PORT      gioPORTA
#define OBC_WD_PIN       6
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

/**
 * @brief MIBSPI for onboard OBC features (flash and RTC).
 */
#define OBC_MIBSPI mibspiREG3

/**
 * @brief RTC pin configuration.
 */
#define RTCB_CS0_PORT   gioPORTA
#define RTCB_CS0_PIN    0
#define RTCA_CS0_PORT   gioPORTB
#define RTCA_CS0_PIN    1
#define RTC_TS_PORT     gioPORTB
#define RTC_TS_PIN      2
#define RTCA_ALARM_PORT gioPORTB
#define RTCA_ALARM_PIN  3
#define RTCB_ALARM_PORT gioPORTA
#define RTCB_ALARM_PIN  1

/**
 * @brief RTC MIBSPI configuration.
 *
 * The RTC uses two transfer groups:
 *     one 8-byte group for time setting/getting
 *     one 2-byte group for generic register accesses.
 */
#define RTC_TIME_MIBSPI_GROUP 0
#define RTC_REG_MIBSPI_GROUP  1
#define RTC_TIME_MIBSPI_REG   OBC_MIBSPI
#define RTC_REG_MIBSPI_REG    OBC_MIBSPI
#define RTC_DATA_FORMAT       2

/**
 * @brief Flash MIBSPI configuration.
 */
#define FLASH_MIBSPI_REG    OBC_MIBSPI
#define FLASH_DATA_FORMAT   0
#define FLASH_1_BYTE_GROUP  2 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP  3 // transfer group with 2 byte length
#define FLASH_4_BYTE_GROUP  4 // transfer group with 4 byte length
#define FLASH_20_BYTE_GROUP 5 // transfer group with 20 byte length

#define FLASH_CS0_PORT gioPORTA
#define FLASH_CS0_PIN  2

/**
 * @brief MRAM MIBSPI configuration.
 */
#define MRAM_RST_PORT hetPORT1
#define MRAM_RST_PIN  7 // weak pulldown active high reset control of MRAM

#define MRAM_MIBSPI_REG   mibspiREG1
#define MRAM_DATA_FORMAT  0 // choice of mibspi config in halcogen
#define MRAM_1_BYTE_GROUP 1 // transfer group with 1 byte length
#define MRAM_2_BYTE_GROUP 2 // transfer group with 2 byte length
#define MRAM_4_BYTE_GROUP 4 // transfer group with 4 byte length
#define MRAM_CS1_PORT     BP_SPI1_CS_1_PORT
#define MRAM_CS1_PIN      BP_SPI1_CS_1_PIN

/**
 * @brief ECC Flash MIBSPI and reset pin configuration.
 */
#define ECC_FLASH_MIBSPI_REG    mibspiREG1
#define ECC_FLASH_DATA_FORMAT   2
#define ECC_FLASH_1_BYTE_GROUP  1 // transfer group with 1 byte length
#define ECC_FLASH_2_BYTE_GROUP  2 // transfer group with 2 byte length
#define ECC_FLASH_5_BYTE_GROUP  0 // transfer group with 5 byte length (erase command + 4 byte address)
#define ECC_FLASH_22_BYTE_GROUP 5 // transfer group with 22 byte length

#define ECC_FLASH_CS0_PORT BP_SPI1_CS_0_PORT
#define ECC_FLASH_CS0_PIN  BP_SPI1_CS_0_PIN

#define ECC_FLASH_RST_PORT hetPORT1
#define ECC_FLASH_RST_PIN  4

/*
 * @brief Flash size configuration
 * @brief Flash size configuration - 16MB
 */
#define BLOCK_SIZE  4096
#define BLOCK_COUNT 4096

/**
 * @brief Temperature sensor I2C and reset pin configuration.
 */
#define OBC_TEMP_RESET_PORT hetPORT1
#define OBC_TEMP_RESET_PIN  11
#define OBC_TEMP_ADDR       0x48

/**
 * @brief GPS pin configuration.
 */
#define BP_nRESET_IN_GPS_PORT mibspiPORT1
#define BP_nRESET_IN_GPS_PIN  8
#define BP_LOW_POWER_GPS_PORT spiPORT4
#define BP_LOW_POWER_GPS_PIN  11
#define GPS_PORT              scilinREG

/**
 * @brief Heartbeat configuration.
 *
 * The heartbeat is provided by a 1 Hz PWM signal generated by the N2HET module.
 */
#define OBC_HEARTBEAT_HET_PIN 26
#define OBC_HEARTBEAT_HET     hetRAM1
#define OBC_HEARTBEAT_PWM     pwm0

/**
 * @brief Backplane pins - not currently used, but the net names exist in the schematic.
 *
 * References between the schematic backplane pin name and the TMS570 pin name.
 *
 * Most of these are not enabled because the pins are not specifically configured in HALCoGEN (yet).
 */

//#define	BP.CAN2RX.GIO
//#define	BP.CAN2TX.GIO
//#define	BP.CAN3RX.GIO
//#define	BP.CAN3TX.GIO

/**
 * @brief Backplane generic SPI pins.
 */
//#define	BP.SPI1.CLK
//#define	BP.SPI1.MOSI.1
#define BP_SPI1_CS_4_PORT mibspiPORT1
#define BP_SPI1_CS_4_PIN  4
#define BP_SPI1_CS_5_PORT mibspiPORT1
#define BP_SPI1_CS_5_PIN  5
//#define	BP.SPI1.MOSI.0
//#define	BP.SPI1.MISO.0
#define BP_SPI1_CS_0_PORT mibspiPORT1
#define BP_SPI1_CS_0_PIN  0
#define BP_SPI1_CS_1_PORT mibspiPORT1
#define BP_SPI1_CS_1_PIN  1
#define BP_SPI1_CS_2_PORT mibspiPORT1
#define BP_SPI1_CS_2_PIN  2
#define BP_SPI3_CS_5_PORT mibspiPORT1
#define BP_SPI3_CS_5_PIN  5

#define BP_SPI3_CS_1_PORT mibspiPORT3
#define BP_SPI3_CS_1_PIN  1

#define BP_SPI4_CS_0_PORT spiPORT4
#define BP_SPI4_CS_0_PIN  0

//#define	BP.SPI5.MISO.1
//#define	BP.SPI5.CLK
#define BP_SPI5_CS_0_PORT mibspiPORT5
#define BP_SPI5_CS_0_PIN  0
//#define	BP.SPI5.MISO.0
//#define	BP.SPI5.MOSI.0
//#define	BP.SPI4.CLK

/**
 * @brief NHET pins on backplane.
 */
//#define	BP.NHET1.02
//#define	BP.NHET1.26
//#define	BP.NHET1.04
//#define	BP.NHET1.07
//#define	BP.NHET1.22
//#define	BP.NHET1.18
//#define	BP.NHET1.20
//#define	BP.NHET1.10
//#define	BP.NHET1.12
//#define	BP.NHET1.14
//#define	BP.NHET1.28
//#define	BP.NHET1.30
//#define	BP.NHET1.09
//#define	BP.NHET1.01

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
 * @brief EPS ADC Pins
 */
#define EPS_SENSE_V_BAT_ADC 4
#define EPS_SENSE_I_BAT_ADC 5

#endif /* ORCA_V1_HARDWAREDEFS_H_ */
