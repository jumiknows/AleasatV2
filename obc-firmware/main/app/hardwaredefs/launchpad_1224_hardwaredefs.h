/**
 * @file launchpad_1224_hardwaredefs.h
 * @brief Hardware definitions for launchpad platform.
 *
 * PURPOSE
 *   - Hardwaredefs files are used to map hardware peripheral names to common names used throughout our
 *     code This allows different peripherals to be used on different boards, with the same codebase
 *   - These files also allow hardware features to be disabled. This is useful if no RTC is connected,
 *     for example
 *
 * INSTRUCTIONS
 *   - When developing software that requires a peripheral register pointer (such as GIOPORTA), define
 *     your own descriptive name for it (BLINKY_GIO_PORT), and use that in your code.
 *       - Use a #define in the appropriate hwdefs file to map BLINKY_GIO_PORT to GIOPORTA
 *       - Assuming the hardware (GIO port A, in this case) is configured in HALCoGen, the mapping
 *         should work.
 *       - Once verified, add the mapping to other platforms' hwdefs, and configure it for them in
 *         HALCoGEN.
 */

#ifndef LAUNCHPAD_1224_HARDWAREDEFS_H_
#define LAUNCHPAD_1224_HARDWAREDEFS_H_

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
#include "reg_mibspi.h"
#include "reg_spi.h"
#include "het.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief General functionality: peripheral names and RTOS configuration.
 */
#define RTI_CLK_SPEED_HZ ((uint32_t)RTI_FREQ * 1000000) // For RTOS tick
#define UART_DEBUG       scilinREG
#define UART_GPS         sciREG
#define I2C              i2cREG1

/**
 * @brief OBC Blinky LED port and pin
 */
#define OBC_BLINKY_PORT GIO_PORT(gioPORTB)
#define OBC_BLINKY_PIN  1

/**
 * @brief IRQ Example setup
 *
 * The interrupt example toggles LEDs when an input pin state changes. It demonstrates interrupt
 * setup.
 */
#define IRQ_EXAMPLE_LED_PORT gioPORTB // Port that will be toggled as an indicator when interrupts occur.
#define IRQ_EXAMPLE_LED_PIN  2
#define SEMAPHORE_IRQ_EDGE   3 // Port with edge notification enabled.
#define SEMAPHORE_IRQ_REG    hetREG1

/**
 * @brief RTC MIBSPI configuration.
 *
 * The RTC uses two transfer groups:
 *     one 8-byte group for time setting/getting
 *     one 2-byte group for generic register accesses.
 */
#define RTC_MIBSPI_REG   mibspiREG1
#define RTC_DATA_FORMAT       1
#define RTC_REG_MIBSPI_GROUP  1
#define RTC_TIME_MIBSPI_GROUP 2 // Group needs length 8
#define RTC_TIMESTAMP_MIBSPI_GROUP 3
#define RTC_ALARM_MIBSPI_GROUP 4

#define RTC_CS_PORT gioPORTA
#define RTC_CS_PIN  5

#define RTC_ALARM_IRQ_N_PORT gioPORTB
#define RTC_ALARM_IRQ_N_PIN  3

#define RTC_TIMESTAMP_TRIGGER_PORT GIO_PORT(hetPORT1)
#define RTC_TIMESTAMP_TRIGGER_PIN  28

#define RTC_RESET_N_PORT GIO_PORT(gioPORTB)
#define RTC_RESET_N_PIN  0 

/**
 * @brief Flash MIBSPI configuration.
 */
#define FLASH_MIBSPI_REG    mibspiREG1
#define FLASH_DATA_FORMAT   2
#define FLASH_1_BYTE_GROUP  0 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP  1 // transfer group with 2 byte length
#define FLASH_5_BYTE_GROUP  2 // transfer group with 5 byte length
#define FLASH_20_BYTE_GROUP 3 // transfer group with 20 byte length
#define FLASH_69_BYTE_GROUP 4 // transfer group with 69 byte length

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
#define MRAM_CS1_PORT     mibspiPORT1
#define MRAM_CS1_PIN      1

/**
 * @brief ECC Flash MIBSPI and reset pin configuration.
 */
#define ECC_FLASH_MIBSPI_REG    mibspiREG1
#define ECC_FLASH_DATA_FORMAT   2
#define ECC_FLASH_1_BYTE_GROUP  1 // transfer group with 1 byte length
#define ECC_FLASH_2_BYTE_GROUP  2 // transfer group with 2 byte length
#define ECC_FLASH_5_BYTE_GROUP  0 // transfer group with 5 byte length (erase command + 4 byte address)
#define ECC_FLASH_22_BYTE_GROUP 5 // transfer group with 22 byte length

#define ECC_FLASH_CS0_PORT mibspiPORT1
#define ECC_FLASH_CS0_PIN  0

#define ECC_FLASH_RST_PORT hetPORT1
#define ECC_FLASH_RST_PIN  4

/*
 * @brief Flash size configuration
 * @brief Flash size configuration - 64KB
 */
#define EXT_FLASH_BLOCK_SIZE  1024
#define EXT_FLASH_BLOCK_COUNT 64

/**
 * @brief Watchdog pins and configuration.
 */
#define WD_PET_PERIOD_MS 20000

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
 * @brief Heartbeat configuration.
 *
 * The heartbeat is provided by a 1 Hz PWM signal generated by the N2HET module.
 */
#define OBC_HEARTBEAT_HET_PIN 26
#define OBC_HEARTBEAT_HET     hetRAM1
#define OBC_HEARTBEAT_PWM     pwm0

/**
 * @brief EPS ADC Pins
 */
#define EPS_SENSE_V_BAT_ADC 4
#define EPS_SENSE_I_BAT_ADC 5

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

#define COMMS_WD_PORT  CAN_PORT(canREG1)
#define COMM_WD_PIN    CAN_PIN_TX

#define COMMS_RST_PORT CAN_PORT(canREG1)
#define COMMS_RST_PIN  CAN_PIN_RX

#define COMMS_INT_REG  hetREG1
#define COMMS_INT_PORT hetPORT1
#define COMMS_INT_PIN  31
#define COMMS_INT_EDGE 0

/**
 * @brief GPS pin configuration.
 */
#define GPS_RST_PORT   CAN_PORT(canREG3)
#define GPS_RST_PIN    CAN_PIN_RX
#define GPS_P1PPS_PORT GIO_PORT(gioPORTA)
#define GPS_P1PPS_PIN  2
#define GPS_STS_PORT   GIO_PORT(gioPORTA)
#define GPS_STS_PIN    6

#define GPS_PWR_EN_PORT  GIO_PORT(gioPORTB)
#define GPS_PWR_EN_PIN   2
#define GPS_PWR_FLT_PORT GIO_PORT(gioPORTA)
#define GPS_PWR_FLT_PIN  7

#define GPS_BAUD_RATE 9600

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
 * @brief IMU enable interface
*/
#define IMU_1_EN_PIN 8
#define IMU_1_EN_PORT GIO_PORT(hetPORT1)

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

#endif /* LAUNCHPAD_1224_HARDWAREDEFS_H_ */
