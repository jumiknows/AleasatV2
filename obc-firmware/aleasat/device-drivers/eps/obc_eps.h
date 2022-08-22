/**
 * @file obc_eps.h
 * @brief Interface to communicate with EnduroSAT EPS via I2C
 */

#ifndef OBC_I2C_H_
#define OBC_I2C_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "tms_i2c.h"

// HAL
#include "hal_stdtypes.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Address of EPS module on I2C bus
#define EPS_ADDR			0x18

/**
 * @brief Command IDs to read float32-valued registers
 */
typedef enum {
    EPS_READ_BATTERY_VOLTAGE     = 1U,  ///< Battery bus voltage (volts)
    EPS_READ_BATTERY_CURRENT     = 2U,  ///< Battery bus current (amps)
    EPS_READ_BCR_VOLTAGE         = 3U,  ///< External battery pack voltage (volts)
    EPS_READ_BCR_CURRENT         = 4U,  ///< External battery pack current (amps)
    EPS_READ_X_VOLTAGE           = 5U,  ///< Solar panel X voltage (volts)
    EPS_READ_XM_CURRENT          = 6U,  ///< Solar panel X- current (amps)
    EPS_READ_XP_CURRENT          = 7U,  ///< Solar panel X+ current (amps)
    EPS_READ_Y_VOLTAGE           = 8U,  ///< Solar panel Y voltage (volts)
    EPS_READ_YM_CURRENT          = 9U,  ///< Solar panel Y- current (amps)
    EPS_READ_YP_CURRENT          = 10U, ///< Solar panel Y+ current (amps)
    EPS_READ_Z_VOLTAGE           = 11U, ///< Solar panel Z voltage (volts)
    EPS_READ_ZM_CURRENT          = 12U, ///< Solar panel Z- current (amps)
    EPS_READ_ZP_CURRENT          = 13U, ///< Solar panel Z+ current (amps)
    EPS_READ_BUS_3V3_CURRENT     = 14U, ///< 3V3 bus current (amps)
    EPS_READ_BUS_5V_CURRENT      = 15U, ///< 5V bus current (amps)
    EPS_READ_LUP_3V3             = 16U, ///< 3V3 latch up fault (volts)
    EPS_READ_LUP_5V              = 17U, ///< 5V latch up fault (volts)
    EPS_READ_MCU_TEMP            = 18U, ///< MCU temperature (degrees celsius)
    EPS_READ_BATTERY_TEMP_C1     = 19U, ///< Battery cell 1 temperature (degrees celsius)
    EPS_READ_BATTERY_TEMP_C2     = 20U, ///< Battery cell 2 temperature (degrees celsius)
    EPS_READ_BATTERY_TEMP_C3     = 21U, ///< Battery cell 3 temperature (degrees celsius)
    EPS_READ_BATTERY_TEMP_C4     = 22U, ///< Battery cell 4 temperature (degrees celsius)
    /* ... */
    EPS_READ_MAX_TEMP_1          = 30U, ///< Max temperature in battery pack 1 (degrees celsius)
    EPS_READ_MAX_TEMP_2          = 31U, ///< Max temperature in battery pack 2 (degrees celsius)
    EPS_READ_MAX_TEMP_3          = 32U, ///< Max temperature in battery pack 3 (degrees celsius)
    EPS_READ_MAX_TEMP_4          = 33U, ///< Max temperature in battery pack 4 (degrees celsius)
    EPS_READ_MIN_TEMP_1          = 34U, ///< Min temperature in battery pack 1 (degrees celsius)
    EPS_READ_MIN_TEMP_2          = 35U, ///< Min temperature in battery pack 2 (degrees celsius)
    EPS_READ_MIN_TEMP_3          = 36U, ///< Min temperature in battery pack 3 (degrees celsius)
    EPS_READ_MIN_TEMP_4          = 37U, ///< Min temperature in battery pack 4 (degrees celsius)
    EPS_READ_TEMP_SENSOR_5       = 38U, ///< External temperature sensor 5 (degrees celsius)
    EPS_READ_TEMP_SENSOR_6       = 39U, ///< External temperature sensor 6 (degrees celsius)
    EPS_READ_TEMP_SENSOR_7       = 40U, ///< External temperature sensor 7 (degrees celsius)
    EPS_READ_RESERVED            = 41U, ///< Reserved for external temperature sensor 8
    /* ... */
    EPS_READ_RBAT_INITIAL        = 45U, ///< Initial battery resistance in milli-ohms
    EPS_READ_RBAT_RAW            = 46U, ///< Raw battery resistance in milli-ohms
    EPS_READ_VBAT_IDEAL          = 47U, ///< Ideal battery voltage in milli-volts
    /* ... */
    EPS_READ_BUS_3V3_VOLTAGE     = 49U, ///< 3V3 bus voltage in volts
    EPS_READ_BUS_5V_VOLTAGE      = 50U  ///< 5V bus voltage in volts
} eps_cmd_read_float_t;

/**
 * @brief Command IDs to read uint16-valued registers
 */
typedef enum {
    EPS_READ_INPUT_CONDITION     = 23U, ///< Input condition bitfield
    EPS_READ_OUTPUT_CONDITIONS_1 = 24U, ///< Output conditions 1 bitfield
    EPS_READ_OUTPUT_CONDITIONS_2 = 25U, ///< Output conditions 2 bitfield
    EPS_READ_POWER_ON_CYCLES     = 26U, ///< Power on cycles counter
    EPS_READ_V_UNDER_VOLTAGE     = 27U, ///< V under voltage event counter
    EPS_READ_V_SHORT_CIRCUIT     = 28U, ///< V short circuit event counter
    EPS_READ_V_OVER_TEMPERATURE  = 29U, ///< V over temperature event counter
    /* ... */
    EPS_READ_SOFTWARE_VERSION    = 42U, ///< Software version
    EPS_READ_DEFAULTS_1          = 43U, ///< Read defaults 1 bitfield
    EPS_READ_DEFAULTS_2          = 44U, ///< Read defaults 2 bitfield
    /* ... */
    EPS_READ_CHARGE_CYCLES       = 48U, ///< Charge cycles counter
    /* ... */
    EPS_READ_UPTIME_LOWER_WORD   = 51U, ///< Lower 16 bits of uptime counter (measured in seconds)
    EPS_READ_UPTIME_HIGHER_WORD  = 52U  ///< Upper 16 bits of uptime counter (measured in seconds)
} eps_cmd_read_int_t;

/**
 * @brief Command IDs to write state registers
 */
typedef enum {
    EPS_WRITE_SW_SELF_LOCK        = 0x00U, ///< Self lock switch
    EPS_WRITE_V_BATT_EN           = 0x01U, ///< Battery bus
    EPS_WRITE_BCR_OUT_EN          = 0x02U, ///< BCR bus
    EPS_WRITE_SHD_3V3             = 0x03U, ///< 3V3 bus
    EPS_WRITE_SHD_5V              = 0x04U, ///< 5V bus
    EPS_WRITE_LUP_3V3             = 0x05U, ///< Latch-up protected 3V3 bus
    EPS_WRITE_LUP_5V              = 0x06U, ///< Latch-up protected 5V bus
    EPS_WRITE_SHD_CHARGE          = 0x07U, ///< Battery charger suppresion/shutdown
    EPS_WRITE_CHARGE_I1           = 0x08U, ///< Fast/slow battery charge 1
    EPS_WRITE_CHARGE_I2           = 0x09U, ///< Fast/slow battery charge 2
    EPS_WRITE_OUT1                = 0x0AU, ///< Output 1 / payload
    EPS_WRITE_OUT2                = 0x0BU, ///< Output 2
    EPS_WRITE_OUT3                = 0x0CU, ///< Output 3
    EPS_WRITE_OUT4                = 0x0DU, ///< Output 4 / OBC
    EPS_WRITE_OUT5                = 0x0EU, ///< Output 5 / UHF
    EPS_WRITE_OUT6                = 0x0FU, ///< Output 6
    EPS_WRITE_HEATER_1            = 0x10U, ///< Battery heater 1
    EPS_WRITE_HEATER_2            = 0x11U, ///< Battery heater 2
    EPS_WRITE_HEATER_3            = 0x12U, ///< Battery heater 3
    EPS_WRITE_I2C_PULLUP_10K      = 0x13U, ///< 10k I2C pullup resistor
    EPS_WRITE_I2C_PULLUP_4K7      = 0x14U, ///< 4.7k I2C pullup resistor
    EPS_WRITE_RS485_TERM_RESISTOR = 0x15U, ///< Enable the 120 ohm Termination Resistor at RS485
} eps_cmd_write_state_t;

/**
 * @brief Write command states
 */
typedef enum {
    AUTO_OFF    = 0U,
    AUTO_ON     = 1U,
    FORCED_OFF  = 2U,
    FORCED_ON   = 3U
} eps_write_state_t;

#define EPS_WRITE_CMD_RBAT_RAW_INITIAL  0x20U  ///< Initial value for internal battery resistance calculations (see user manual)

/**
 * @brief Return value for EPS functions
 */
typedef enum {
    /**
     * @brief EPS function succeeded
     */
    EPS_SUCCESS = 0,

    /**
     * @brief I2C transaction returned an error
     */
    EPS_I2C_ERROR = -1,

    /**
     * @brief Invalid command was passed to function
     */
    EPS_INVALID_COMMAND = -2
} eps_err_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Read APIs

eps_err_t eps_read_float(eps_cmd_read_float_t cmd, float32* data);
eps_err_t eps_read_float_raw(eps_cmd_read_float_t cmd, uint16_t* data);
eps_err_t eps_read_int(eps_cmd_read_int_t cmd, uint16_t* data);

// Write APIs
eps_err_t eps_write_state(eps_cmd_write_state_t cmd, eps_write_state_t state);
eps_err_t eps_write_initial_resistance(uint8_t rbat_raw);

#endif // OBC_I2C_H_

