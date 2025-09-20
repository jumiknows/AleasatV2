/**
 * @file pca2129_rtc.c
 * @brief Driver for the PCA2129 RTC
*/

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "pca2129_rtc.h"

// OBC
#include "obc_rtc.h"
#include "obc_hardwaredefs.h"
#include "obc_gpio.h"
#include "logger.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define RTC_MIBSPI_TIMEOUT_MS       30
#define TIMESTAMP_CAPTURE_PERIOD_MS 20

// PCA2129 command fields

#define PCA2129_READ            0b1
#define PCA2129_WRITE           0b0
#define SUBADDRESS              0b1
#define READ_REGISTER   (uint8_t)((PCA2129_READ << 7) | (SUBADDRESS << 5))
#define WRITE_REGISTER  (uint8_t)((PCA2129_WRITE << 7) | (SUBADDRESS << 5))

#define PCA2129_ALARM_X_ENABLE  0b0
#define PCA2129_ALARM_X_DISABLE 0b1

// PCA2129 control registers

#define RTC_CTRL1               0x00
#define RTC_CTRL2               0x01
#define RTC_CTRL3               0x02

/**
 * Mask for the write-clearable flag bits in control registers
 * 1 and 2 of the PCA2129
 */
#define RTC_CTRL1_WR_CLR_FLAGS  (uint8_t) 0b00010000
#define RTC_CTRL2_WR_CLR_FLAGS  (uint8_t) 0b10110000

#define TIMESTAMP_CTL           0x12

#define CLOCKOUT_CTL            0x0F
#define CLOCKOUT_CTL_OTPR_BIT   5

/**
 * Disable clockout programmable square wave by writing 0b111 to
 * the COF[2:0] bits (see section 7.3 of the PCA2129 datasheet)
 */
#define CLOCKOUT_DISABLE        0b00000111

// PCA2129 clock registers

#define CLOCK_SECONDS       0x03
#define CLOCK_MINUTES       0x04
#define CLOCK_HOURS         0x05
#define CLOCK_DAYS          0x06
#define CLOCK_WEEKDAYS      0x07
#define CLOCK_MONTHS        0x08
#define CLOCK_YEARS         0x09

// PCA2129 timestamp registers

#define TIMESTAMP_SECOND    0x13
#define TIMESTAMP_MINUTE    0x14
#define TIMESTAMP_HOUR      0x15
#define TIMESTAMP_DAY       0x16
#define TIMESTAMP_MONTH     0x17
#define TIMESTAMP_YEAR      0x18

// PCA2129 alarm registers

#define ALARM_SECOND    0x0A
#define ALARM_MINUTE    0x0B
#define ALARM_HOUR      0x0C
#define ALARM_DAY       0x0D
#define ALARM_WEEKDAY   0x0E

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/* One byte */
typedef uint8_t one_byte_t;

/* Five bytes */
typedef struct five_bytes {
    uint8_t bytes[5];
} five_bytes_t;

/* Six bytes */
typedef struct six_bytes {
    uint8_t bytes[6];
} six_bytes_t;

/* Seven bytes */
typedef struct seven_bytes {
    uint8_t bytes[7];
} seven_bytes_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static const mibspi_tg_t rtc_2byte_tg = {.reg = RTC_MIBSPI_REG, .transfer_group = RTC_REG_MIBSPI_GROUP, .cs_port = RTC_CS_PORT, .cs_pin = RTC_CS_PIN};
static const mibspi_tg_t rtc_6byte_tg = {.reg = RTC_MIBSPI_REG, .transfer_group = RTC_ALARM_MIBSPI_GROUP, .cs_port = RTC_CS_PORT, .cs_pin = RTC_CS_PIN};
static const mibspi_tg_t rtc_7byte_tg = {.reg = RTC_MIBSPI_REG, .transfer_group = RTC_TIMESTAMP_MIBSPI_GROUP, .cs_port = RTC_CS_PORT, .cs_pin = RTC_CS_PIN};
static const mibspi_tg_t rtc_8byte_tg = {.reg = RTC_MIBSPI_REG, .transfer_group = RTC_TIME_MIBSPI_GROUP, .cs_port = RTC_CS_PORT, .cs_pin = RTC_CS_PIN};

// PCA2129 configuration (refer to section 7.2 of the PCA2129 datasheet for more info)

/**
 * @brief Configuration byte for control register 1
 *
 *  bit   | value |    action
 * ----------------------------------------
 *   7    |   0   | use PCA2129's internal clock
 *   6    |   0   | N/A (UNUSED)
 *   5    |   0   | run RTC source clock
 *   4    |   0   | TSF1 clear (is set when the nTS input to the PCA2129 is driven low)
 *   3    |   0   | POWER-ON RESET OVERRIDE DISABLED (disable for normal operation)
 *   2    |   0   | 24-hr mode
 *   1    |   0   | minute interrupt disabled
 *   0    |   0   | second interrupt disabled
*/
static const one_byte_t pca2129_config_ctrl1 = 0x00;

/**
 * @brief configuration byte for control register 2 value
 *
 *  bit   | value |    action
 * ----------------------------------------
 *   7    |   0   | MSF clear (is set when the PCA2129 generates a minute or second interupt)
 *   6    |   0   | WDTF clear (is set when the PCA2129 generates a watchdog timer interrup or reset)
 *   5    |   0   | TSF2 clear (is set when a rising or falling edge is detected in the nTS input to the PCA2129)
 *   4    |   0   | AF clear (is set when the PCA2129 generates an alarm interrupt)
 *   3    |   0   | N/A (UNUSED)
 *   2    |   0   | timestamp interrupt disabled
 *   1    |   1   | alarm interrupt enabled
 *   0    |   0   | N/A (UNUSED)
*/
static const one_byte_t pca2129_config_ctrl2 = 0x02;

/**
 * @brief configuration byte for control register 2 value
 *
 *  bit   | value |    action
 * ----------------------------------------
 *   7    |   1   | battery switch-over disabled, single power supply (V_DD, no V_BAT), battery low detection disabled
 *   6    |   1   |     "
 *   5    |   1   |     "
 *   4    |   0   | battery switch-over timestamp disabled
 *   3    |   0   | BF clear (is set when the PCA2129 generates a battery switch-over interrupt)
 *   2    |   0   | BLF clear (is set when PCA2129 detects low battery)
 *   1    |   0   | battery switch-over interrupt disabled
 *   0    |   0   | low battery interrupt disabled
*/
static const one_byte_t pca2129_config_ctrl3 = 0xE0;

/**
 * @brief timestamp functionality configuration
 *
 *  bit   | value |    action
 * ----------------------------------------
 *   7    |   0   | timestamp mode: time stamp last timestamp event that occured while timestamp flags remained set.
 *   6    |   0   | timestamp function enabled
 *   5    |   0   | N/A (UNUSED)
 *   4    |   0   | 1/16 th second timestamp, BCD format
 *   3    |   0   |             "
 *   2    |   0   |             "
 *   1    |   0   |             "
 *   0    |   0   |             "
*/
static const one_byte_t pca2129_timestamp_config = 0x00;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void pca2129_mibspi_init(void);
static bool pca2129_oscillator_stop_occurred(void);
static void pca2129_reset(void);
static void pca2129_reset_alarm_registers(void);
static rtc_err_t pca2129_disable_clockout(void);
static rtc_err_t pca2129_write_single_bit(const uint8_t reg, const uint8_t bit_to_write, const uint8_t val);

static rtc_err_t pca2129_read_single_reg(const uint8_t reg, one_byte_t *rx_data);
static rtc_err_t pca2129_read_six_regs_continuous(const uint8_t start_reg, six_bytes_t *rx_data);
static rtc_err_t pca2129_read_seven_regs_continuous(const uint8_t start_reg, seven_bytes_t *rx_data);

static rtc_err_t pca2129_write_single_reg(const uint8_t reg, const one_byte_t *tx_data);
static rtc_err_t pca2129_write_five_regs_continuous(const uint8_t start_reg, const five_bytes_t *tx_data);
static rtc_err_t pca2129_write_seven_regs_continuous(const uint8_t start_reg, const seven_bytes_t *tx_data);

static uint8_t int_to_bcd(uint8_t num);
static uint8_t bcd_to_int(uint8_t bcd);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Start the RTC driver.
 *
 * @returns RTC_CONFIG_SUCCESS if the PCA2129 is re-configured. RTC_SUCCESS otherwise.
*/
rtc_err_t pca2129_rtc_init(void) {
    pca2129_mibspi_init();

    /**
     * Reset and re-configure the RTC here, only if there has been a RTC
     * chip reset (for example, when the PCA2129 is first powered on), which
     * is independent from OBC resets. Otherwise, the RTC's timekeeping should
     * persist uninterrupted across OBC resets.
     */
    if (!pca2129_oscillator_stop_occurred()) {
        return RTC_SUCCESS;
    }

    pca2129_reset();
    pca2129_write_single_bit((uint8_t) CLOCK_SECONDS, 7, 0); // clear the PCA2129's oscillator stop flag (OSF) after the reset

    // configure RTC
    pca2129_reset_alarm_registers();
    pca2129_write_single_reg((uint8_t) TIMESTAMP_CTL, &pca2129_timestamp_config);
    pca2129_write_single_reg((uint8_t) RTC_CTRL1, &pca2129_config_ctrl1);
    pca2129_write_single_reg((uint8_t) RTC_CTRL2, &pca2129_config_ctrl2);
    pca2129_write_single_reg((uint8_t) RTC_CTRL3, &pca2129_config_ctrl3);
    pca2129_disable_clockout();

    pca2129_set_current_time(&alea_time_init);
    return RTC_CONFIG_SUCCESS;
}

/**
 * @brief Set the current time in the RTC (excluding weekday).
 *
 * @param curr_time: A real_time_t struct. Contains second, minute, hour, day, month, and year fields.
 * @returns RTC_SUCCESS if no errors. Error code otherwise.
*/
rtc_err_t pca2129_set_current_time(const real_time_t *curr_time) {
    seven_bytes_t tx_data;
    tx_data.bytes[0] = int_to_bcd(curr_time->second);
    tx_data.bytes[1] = int_to_bcd(curr_time->minute);
    tx_data.bytes[2] = int_to_bcd(curr_time->hour);
    tx_data.bytes[3] = int_to_bcd(curr_time->day);
    tx_data.bytes[4] = 0x00; // Weekday, ignore.
    tx_data.bytes[5] = int_to_bcd(curr_time->month);
    tx_data.bytes[6] = int_to_bcd(curr_time->year);
    return pca2129_write_seven_regs_continuous((uint8_t) CLOCK_SECONDS, &tx_data);
}

/**
 * @brief Get the current time from the RTC.
 *
 * @param curr_time: A real_time_t struct. Contains second, minute, hour, day, month, and year fields.
 * @returns RTC_SUCCESS if no errors. Error code otherwise.
*/
rtc_err_t pca2129_get_current_time(real_time_t *curr_time) {
    rtc_err_t err = RTC_SUCCESS;
    seven_bytes_t rx_data;

    err = pca2129_read_seven_regs_continuous((uint8_t) CLOCK_SECONDS, &rx_data);

    curr_time->second   = bcd_to_int(rx_data.bytes[0]);
    curr_time->minute   = bcd_to_int(rx_data.bytes[1]);
    curr_time->hour     = bcd_to_int(rx_data.bytes[2]);
    curr_time->day      = bcd_to_int(rx_data.bytes[3]);
    curr_time->month    = bcd_to_int(rx_data.bytes[5]);
    curr_time->year     = bcd_to_int(rx_data.bytes[6]);

    return err;
}

/**
 * @brief Set the RTC alarm callback and arguments,
 * and set an alarm in the PCA2129 with second, minute, and hour alarm parameters enabled.
 *
 * @param second: second when the alarm should be triggered.
 * @param minute: minute when the second alarm is scheduled.
 * @param hour: hour when the minute alarm is scheduled.
 * @param returns RTC_SUCCESS if no errors, error code otherwise.
*/
rtc_err_t pca2129_set_absolute_alarm(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day) {
    five_bytes_t tx_data;

    tx_data.bytes[0] = (PCA2129_ALARM_X_ENABLE << 7) | int_to_bcd(second);
    tx_data.bytes[1] = (PCA2129_ALARM_X_ENABLE << 7) | int_to_bcd(minute);
    tx_data.bytes[2] = (PCA2129_ALARM_X_ENABLE << 7) | int_to_bcd(hour);
    tx_data.bytes[3] = (PCA2129_ALARM_X_ENABLE << 7) | int_to_bcd(day);
    tx_data.bytes[4] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;   // Weekday alarm field
    return pca2129_write_five_regs_continuous((uint8_t) ALARM_SECOND, &tx_data);
}

/**
 * @brief Clear the RTC's alarm flag and reset its alarm registers.
*/
void pca2129_clear_alarm(void) {
    pca2129_write_single_bit(RTC_CTRL2, 4, 0); // Clear alarm flag and interrupt signal from the PCA2129
    pca2129_reset_alarm_registers();
}

/**
 * @brief Trigger the PCA2129 to record a timestamp.
 *
 * @returns RTC_SUCCESS if no errors. Error code otherwise.
*/
rtc_err_t pca2129_capture_timestamp(void) {
    // pulse nTS pin of the PCA2129 to make it record the current time as a timestamp
    if (obc_gpio_write(RTC_TIMESTAMP_TRIGGER_PORT, RTC_TIMESTAMP_TRIGGER_PIN, 0) != GPIO_SUCCESS) {
        return RTC_GPIO_WRITE_ERR;
    }

    vTaskDelay(pdMS_TO_TICKS(TIMESTAMP_CAPTURE_PERIOD_MS));

    if (obc_gpio_write(RTC_TIMESTAMP_TRIGGER_PORT, RTC_TIMESTAMP_TRIGGER_PIN, 1) != GPIO_SUCCESS) {
        return RTC_GPIO_WRITE_ERR;
    }

    // reset TSF1 flag in Control Register 1 of the PCA2129 (1 of 2 flags to clear to enable next timestamp)
    if (pca2129_write_single_bit((uint8_t) RTC_CTRL1, 4, 0) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    // reset TSF2 flag in Control Register 2 of the PCA2129 (2 of 2 flags to clear to enable next timestamp)
    if (pca2129_write_single_bit((uint8_t) RTC_CTRL2, 5, 0) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Read the date and time recorded in the RTC's timestamp registers.
 *
 * @pre: @ref pca2129_capture_timestamp() should be called first.
 * @param ts: A real_time_t struct. Contains second, minute, hour, day, month, and year fields.
 * @returns RTC_SUCCESS if no errors. Error code otherwise.
*/
rtc_err_t pca2129_read_timestamp(real_time_t *ts) {
    six_bytes_t rx_data;

    if (pca2129_read_six_regs_continuous((uint8_t) TIMESTAMP_SECOND, &rx_data) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    ts->second  = bcd_to_int(rx_data.bytes[0]);
    ts->minute  = bcd_to_int(rx_data.bytes[1]);
    ts->hour    = bcd_to_int(rx_data.bytes[2]);
    ts->day     = bcd_to_int(rx_data.bytes[3]);
    ts->month   = bcd_to_int(rx_data.bytes[4]);
    ts->year    = bcd_to_int(rx_data.bytes[5]);

    return RTC_SUCCESS;
}

/**
 * @brief Read a single RTC register for debug purposes.
 *
 * @param reg: PCA2129 register to read
 * @param rx_data: Buffer to read data into
 * @returns RTC_SUCCESS if no error. Error code otherwise.
*/
rtc_err_t pca2129_debug_read_single_register(const uint8_t reg, uint8_t *val) {
    rtc_err_t err = RTC_SUCCESS;
    one_byte_t rx_data;

    err = pca2129_read_single_reg(reg, &rx_data);
    *val = (uint8_t) rx_data;

    return err;
}

/**
 * @brief Write a single RTC register for debug purposes.
 *
 * @param reg: PCA2129 register to write
 * @param rx_data: Buffer containing data to write
 * @returns RTC_SUCCESS if no error. Error code otherwise.
*/
rtc_err_t pca2129_debug_write_single_register(const uint8_t reg, const uint8_t *val) {
    one_byte_t tx_data = (one_byte_t) * val;
    return pca2129_write_single_reg(reg, &tx_data);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Initialize the MiBSPI port and transfer groups used by the RTC.
*/
static void pca2129_mibspi_init(void) {
    mibspiEnableGroupNotification(RTC_MIBSPI_REG, RTC_REG_MIBSPI_GROUP, 0U);
    mibspiEnableGroupNotification(RTC_MIBSPI_REG, RTC_TIME_MIBSPI_GROUP, 0U);
    mibspiEnableGroupNotification(RTC_MIBSPI_REG, RTC_TIMESTAMP_MIBSPI_GROUP, 0U);
    mibspiEnableGroupNotification(RTC_MIBSPI_REG, RTC_ALARM_MIBSPI_GROUP, 0U);
}

/**
 * @brief Reset the PCA2129.
*/
static void pca2129_reset(void) {
    // pulse reset pin
    obc_gpio_write(RTC_RESET_N_PORT, RTC_RESET_N_PIN, 0);
    obc_gpio_write(RTC_RESET_N_PORT, RTC_RESET_N_PIN, 1);

    // wait 2 seconds for reset to complete
    vTaskDelay(pdMS_TO_TICKS(2000));

    // OTP refresh
    pca2129_write_single_bit((uint8_t) CLOCKOUT_CTL, CLOCKOUT_CTL_OTPR_BIT, 0);
    pca2129_write_single_bit((uint8_t) CLOCKOUT_CTL, CLOCKOUT_CTL_OTPR_BIT, 1);

    // wait 100 ms for OTP refresh to complete
    vTaskDelay(pdMS_TO_TICKS(100));
}

/**
 * @brief Disable all alarm registers. They will be enabled as needed when an alarm is set.
*/
static void pca2129_reset_alarm_registers(void) {
    five_bytes_t tx_data;
    tx_data.bytes[0] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;
    tx_data.bytes[1] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;
    tx_data.bytes[2] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;
    tx_data.bytes[3] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;
    tx_data.bytes[4] = (PCA2129_ALARM_X_DISABLE << 7) | 0x00;
    pca2129_write_five_regs_continuous((uint8_t) ALARM_SECOND, &tx_data);
}

/**
 * @brief Check if there was a PCA2129 chip reset since the last time its
 * oscillator stop flag (OSF) was cleared. Clears the OSF if is set.
 *
 * @return TRUE if OSF was set. FALSE otherwise.
 */
static bool pca2129_oscillator_stop_occurred(void) {
    one_byte_t reg_val;
    bool oscillator_stop_occurred = false;

    pca2129_read_single_reg((uint8_t) CLOCK_SECONDS, &reg_val);

    if ((uint8_t) reg_val & 0x80) {
        oscillator_stop_occurred = true;
        pca2129_write_single_bit((uint8_t) CLOCK_SECONDS, 7, 0);
    }

    return oscillator_stop_occurred;
}

static rtc_err_t pca2129_disable_clockout(void) {
    one_byte_t clkout_val_initial;
    one_byte_t clkout_val_to_write;

    if (pca2129_read_single_reg((uint8_t) CLOCKOUT_CTL, &clkout_val_initial) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    clkout_val_to_write = clkout_val_initial | CLOCKOUT_DISABLE;

    if (pca2129_write_single_reg((uint8_t) CLOCKOUT_CTL, &clkout_val_to_write) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Write a single bit in any chosen PCA2129 register.
 *
 * @param reg: Register to write to.
 * @param bit_position: Which bit in the register to modify. Range is from bit 0 to bit 7.
 * @param val: The target value of the bit to modify. Must be 0x00 or 0x01.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_write_single_bit(const uint8_t reg, const uint8_t bit_position, const uint8_t val) {
    one_byte_t reg_value_initial;
    one_byte_t reg_value_to_write;

    if (val != 0x00 && val != 0x01) {
        return RTC_BIT_WRITE_ERR;
    }

    if (bit_position > 7) {
        return RTC_BIT_WRITE_ERR;
    }

    if (pca2129_read_single_reg((uint8_t) reg, &reg_value_initial) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    /**
     * The PCA2129's control register 2 (RTC_CTRL2) has multiple write-clearable flag bits.
     * To avoid accidentally clearing flags we don't intend to, we need to write 1's to them,
     * regardless of prior value.
     *
     * How might we accidentally clear flags? When we read from the RTC_CTRL2, modify a single
     * bit, and write it back, we might read a 0 for a flag and write it back. However, the flag
     * could have changed to 1 between reading and writing it. So writing back the value we read
     * would unwittingly clear the flag. To handle this case, the PCA2129 writes
     *
     *                      [incoming value] AND [current value]
     *
     * to flags, making it so **writing 1 has no effect** and writing 0 changes a flag's value to 0,
     * clearing it. That's why when we don't want to modify a flag, we write 1 to it.
     */
    if (reg == (uint8_t) RTC_CTRL2) {
        // Pretend we read 1's from all flags, so we don't need to specially write 1's to flags we didn't intend to clear.
        reg_value_initial |= RTC_CTRL2_WR_CLR_FLAGS;
    } else if (reg == (uint8_t) RTC_CTRL1) {
        // ... Similarly for control register 1 ...
        reg_value_initial |= RTC_CTRL1_WR_CLR_FLAGS;
    }

    if (val == 0x01) {
        reg_value_to_write = reg_value_initial | (1 << bit_position);
    } else {
        reg_value_to_write = reg_value_initial & ~(1 << bit_position);
    }

    if (pca2129_write_single_reg((uint8_t) reg, &reg_value_to_write) != RTC_SUCCESS) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Read from an RTC register.
 *
 * @param reg: The RTC register to read. Addresses between 0x00 and 0x1B inclusive are valid.
 * @param rx_data: 1-byte buffer into which data from the RTC register
 * will be read.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_read_single_reg(const uint8_t reg, one_byte_t *rx_data) {
    uint16_t tx_buffer[2] = {(READ_REGISTER | reg), 0x00};
    uint16_t rx_buffer[2] = { 0x00 };

    if (tms_mibspi_tx_rx(&rtc_2byte_tg, tx_buffer, rx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    *rx_data = (uint8_t) rx_buffer[1];

    return RTC_SUCCESS;
}

/**
 * @brief Read from six consecutive RTC registers.
 *
 * @param start_reg: The RTC register to start reading from. Addresses between 0x00 and 0x1B inclusive are valid.
 * @param rx_data: 6-byte buffer into which data from the RTC register will be read.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_read_six_regs_continuous(const uint8_t start_reg, six_bytes_t *rx_data) {
    uint16_t tx_buffer[7] = {(READ_REGISTER | start_reg), [1 ... 6] = 0x00};
    uint16_t rx_buffer[7] = { 0x00 };

    if (tms_mibspi_tx_rx(&rtc_7byte_tg, tx_buffer, rx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    rx_data->bytes[0] = (uint8_t) rx_buffer[1];
    rx_data->bytes[1] = (uint8_t) rx_buffer[2];
    rx_data->bytes[2] = (uint8_t) rx_buffer[3];
    rx_data->bytes[3] = (uint8_t) rx_buffer[4];
    rx_data->bytes[4] = (uint8_t) rx_buffer[5];
    rx_data->bytes[5] = (uint8_t) rx_buffer[6];

    return RTC_SUCCESS;
}

/**
 * @brief Read from seven consecutive RTC registers.
 *
 * @param start_reg: The RTC register to start reading from. Addresses between 0x00 and 0x1B inclusive are valid.
 * @param rx_data: 7-byte buffer into which data from the RTC register will be read.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_read_seven_regs_continuous(const uint8_t start_reg, seven_bytes_t *rx_data) {
    uint16_t tx_buffer[8] = {(READ_REGISTER | start_reg), [1 ... 7] = 0x00};
    uint16_t rx_buffer[8] = { 0x00 };

    if (tms_mibspi_tx_rx(&rtc_8byte_tg, tx_buffer, rx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    rx_data->bytes[0] = (uint8_t) rx_buffer[1];
    rx_data->bytes[1] = (uint8_t) rx_buffer[2];
    rx_data->bytes[2] = (uint8_t) rx_buffer[3];
    rx_data->bytes[3] = (uint8_t) rx_buffer[4];
    rx_data->bytes[4] = (uint8_t) rx_buffer[5];
    rx_data->bytes[5] = (uint8_t) rx_buffer[6];
    rx_data->bytes[6] = (uint8_t) rx_buffer[7];

    return RTC_SUCCESS;
}

/**
 * @brief Write to an RTC register.
 *
 * @param reg: The RTC register to write. Addresses between 0x00 and 0x1B inclusive are valid.
 * @param tx_data: 1-byte buffer containing data to be written to the RTC register.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_write_single_reg(const uint8_t reg, const one_byte_t *tx_data) {
    uint16_t tx_buffer[2] = {(WRITE_REGISTER | reg), *tx_data};

    if (tms_mibspi_tx(&rtc_2byte_tg, tx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Write to five consecutive RTC registers.
 *
 * @param reg: The RTC register to start writing from. Addresses between 0x00 and 0x17 inclusive are valid.
 * @param tx_data: 5-byte buffer containing data to be written to the RTC registers.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_write_five_regs_continuous(const uint8_t start_reg, const five_bytes_t *tx_data) {
    uint16_t tx_buffer[6] = {(WRITE_REGISTER | start_reg),
                             tx_data->bytes[0],
                             tx_data->bytes[1],
                             tx_data->bytes[2],
                             tx_data->bytes[3],
                             tx_data->bytes[4]
                            };

    if (tms_mibspi_tx(&rtc_6byte_tg, tx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Write to seven consecutive RTC registers.
 *
 * @param reg: The RTC register to start writing from. Addresses between 0x00 and 0x15 inclusive are valid.
 * @param tx_data: 7-byte buffer containing data to be written to the RTC registers.
 * @return RTC_SUCCESS if no error. Error code otherwise.
*/
static rtc_err_t pca2129_write_seven_regs_continuous(const uint8_t start_reg, const seven_bytes_t *tx_data) {
    uint16_t tx_buffer[8] = {(WRITE_REGISTER | start_reg),
                             tx_data->bytes[0],
                             tx_data->bytes[1],
                             tx_data->bytes[2],
                             tx_data->bytes[3],
                             tx_data->bytes[4],
                             tx_data->bytes[5],
                             tx_data->bytes[6],
                            };

    if (tms_mibspi_tx(&rtc_8byte_tg, tx_buffer, RTC_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        return RTC_MIBSPI_ERR;
    }

    return RTC_SUCCESS;
}

/**
 * @brief Encode a two-digit decimal number in binary-coded decimal (BCD) format.
 *
 * @param num: Two-digit number to be encoded in BCD format.
 * @return @ref num in BCD format.
*/
static uint8_t int_to_bcd(uint8_t num) {
    return ((num / 10) << 4) | (num % 10);
}

/**
 * @brief Decode two-digit decimal value from binary-coded decimal (BCD) form.
 *
 * @param bcd: Two-digit decimal number in binary-coded decimal form.
 * @return Decimal equivalent of @ref bcd.
*/
static uint8_t bcd_to_int(uint8_t bcd) {
    return (0xF & bcd) + 10 * (bcd >> 4);
}

