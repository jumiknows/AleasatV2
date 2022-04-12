/*
 * rtc_pca2129.c
 *
 *  Created on: Jan 19, 2019
 *      Author: Julie Belleville
 *
 *  Edited on: May 19, 2019
 * 		Author: Andrada Zoltan
 * 		Changes: - changed API to use real time and not epoch
 * 		         - added support to specify which RTC to use
 *
 *  Edited on: Jan 20 - Jan 26, 2020
 *  		Author: Richard Arthurs
 *  		Changes: - Adjusted year reads/writes to work with the RTC's 0-99 range.
 *  				 - Added obc_time.h to factor out some time-related functions and defs. Factored
 * out is_leap_year().
 *  				 - Made MISRA compliant.
 *  				 - Fixed | vs || bug in alarm setting functions.
 */

#ifdef PLATFORM_ORCA_V1 // TODO(ALEA-511): left reference until ALEA-357 RTC Driver is completed

#include "obc_time.h"
#include "rtos.h"
#include "rtc_pca2129.h"
#include "obc_hardwaredefs.h"
#include "rtc_pca2129_defs.h"
#include "obc_mibspi.h"
#include "logger.h"
#include "printf.h"

/* External Variable Initializations -----------------------------------------*/
rtc_t rtc_a = {
    .prev_rtc_epoch = NO_EPOCH, .start_time = {0}, .cs_port = RTCA_CS0_PORT, .cs_pin = RTCA_CS0_PIN, .alarm_port = RTCA_ALARM_PORT, .alarm_pin = RTCA_ALARM_PIN, .name = "A", .log_id = LOG_RTC_A_LOG};

rtc_t rtc_b = {
    .prev_rtc_epoch = NO_EPOCH, .start_time = {0}, .cs_port = RTCB_CS0_PORT, .cs_pin = RTCB_CS0_PIN, .alarm_port = RTCB_ALARM_PORT, .alarm_pin = RTCB_ALARM_PIN, .name = "B", .log_id = LOG_RTC_B_LOG};

/* Private Function Prototypes -----------------------------------------------*/
static rtc_err_t pca2129_alarm_time_valid(real_time_t curr_time, real_time_t alarm_time);
static uint8_t pca2129_compare_time_in_month(real_time_t ref_time, real_time_t time_to_compare);
static uint16_t pca2129_bcd_to_bin(uint8_t val);
static uint8_t pca2129_bin_to_bcd(uint16_t val);
static rtc_err_t pca2129_set_bit(rtc_t* rtc, uint8_t reg, uint8_t bit_to_set, uint8_t val);
static rtc_err_t pca2129_get_bit(rtc_t* rtc, uint8_t reg, uint8_t bit_to_get, uint8_t* val);
static rtc_err_t pca2129_write_register(rtc_t* rtc, uint8_t reg, uint8_t val);
static rtc_err_t pca2129_read_register(rtc_t* rtc, uint8_t reg, uint8_t* curr_reg);

/* Useful Variables ----------------------------------------------------------*/
const uint8_t days_in_month_no_leap[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint8_t days_in_month_leap[]    = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* API Functions -------------------------------------------------------------*/

/**
 * pca2129_rtc_init
 *
 * Initializes the RTC device to default settings, and checks
 * its startup status.
 *
 * rtc: 	   which RTC to initialize
 * returns:    RTC_NO_ERR if successful, otherwise RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR
 */
rtc_err_t pca2129_init(rtc_t* rtc) {
    rtc_err_t err = RTC_NO_ERR;

    // Enable standard mode battery switch over, disable battery low detection
    err = pca2129_write_register(rtc, CTRL_3, 0b00100000);

    // Perform OTP refresh to load calibration parameters
    err = pca2129_set_bit(rtc, CLKOUT_CTL, OTPR, 0);
    err = pca2129_set_bit(rtc, CLKOUT_CTL, OTPR, 1);
    vTaskDelay(pdMS_TO_TICKS(100)); // OTP refresh takes 100ms to occur

    // Disable CLKOUT feature
    err = pca2129_write_register(rtc, CLKOUT_CTL, 0b00000111);

    // Set temperature measurement check frequency to 1 minute
    err = pca2129_set_bit(rtc, CLKOUT_CTL, TCR_1, 1);
    err = pca2129_set_bit(rtc, CLKOUT_CTL, TCR_0, 0);

    // Check RTC startup status
    err = pca2129_get_startup_status(rtc);

    // Set initialization time
    real_time_t* start_time = &(rtc->start_time); // Directly passing &rtc->start_time to
                                                  // get_current_time() raises a MISRA warning.
    err = pca2129_get_current_time(rtc, start_time);

    // Initialize the previous time to the start time
    rtc->prev_rtc_epoch = rtc->start_time.epoch;

    // Watchdog timer is initialized to 'bx, zero it out
    err = pca2129_write_register(rtc, WATCHDG_TIM_VAL, 0b00000000);

    // Ensure the timestamp is released
    pca2129_timestamp_set(PCA2129_TIMESTAMP_RELEASE);

    rtc->most_recent_low_level_err = err;
    return err;
}

/**
 * @brief Get the current real time from the RTC using the time registers.
 *
 * @param[inout] rtc Which RTC to get time from.
 * @param[out] curr_time Output argument to store the read real time.
 * @return RTC_NO_ERR if successful, otherwise RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR.
 */
rtc_err_t pca2129_get_current_time(rtc_t* rtc, real_time_t* curr_time) {
    curr_time->epoch = no_epoch; // Invalidate the epoch since we're about to update it

    /* The RTC will read out register values consecutively if a clock is provided for readout.
     * To keep the clock running, transmit dummy values for the amount of bytes intended to be
     * read from the RTC.
     */
    uint16_t rx_buffer[8] = {0}; // Time read is 7 bytes + 1 idle cycle due to command transmission
    uint16_t tx_buffer[8] = {RTC_READ | SUB_ADDRESS | CLOCK_SECONDS, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    mibspi_err_t err = MIBSPI_NO_ERR;
    rtc_err_t ret    = RTC_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(200))) {
        gioPORT_t* port       = rtc->cs_port;
        uint32_t pin          = rtc->cs_pin;
        mibspi_tg_t rtc_ts_tg = {.reg = RTC_TIME_MIBSPI_REG, .transfer_group = RTC_TIME_MIBSPI_GROUP, .cs_port = port, .cs_pin = pin};

        // Read the current time
        err = mibspi_transmit_receive(rtc_ts_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return RTC_MIBSPI_MUTEX_GRAB_ERR;
    }

    /* After the transmit completes, the read buffer should look as follows:
     * 		idle value | seconds | minutes | hours | days | weekdays | months | years
     */
    curr_time->second = pca2129_bcd_to_bin(rx_buffer[1] & 0x7F);
    curr_time->minute = pca2129_bcd_to_bin(rx_buffer[2] & 0x7F);
    curr_time->hour   = pca2129_bcd_to_bin(rx_buffer[3] & 0x3F);
    curr_time->day    = pca2129_bcd_to_bin(rx_buffer[4] & 0x3F);
    curr_time->month  = pca2129_bcd_to_bin(rx_buffer[6] & 0x1F);
    curr_time->year   = pca2129_bcd_to_bin(rx_buffer[7]);

    // Set the generic MIBSPI error if one occurred
    ret = ((err != MIBSPI_NO_ERR) ? RTC_MIBSPI_ERR : RTC_NO_ERR);

    // Calculate the epoch if no MIBSPI errors occurred
    if (ret == RTC_NO_ERR) {
        ret = (set_real_time_epoch(curr_time) ? RTC_NO_ERR : RTC_EPOCH_INVALID);
    }

    rtc->most_recent_low_level_err = ret;
    return ret;
}

/**
 * @brief Get the current real time from the RTC using the timestamp.
 * @pre The timestamp pin is held low by the caller. If using multiple RTCs, the timestamp pin
 * should be held low and not released until the last RTC has been read.
 *
 * The time and the epoch in the real_time structure will both be populated.
 *
 * @param[inout] rtc Which RTC to get time from.
 * @param[out] curr_time Output argument to store the read real time.
 * @return RTC_NO_ERR if successful, otherwise RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR.
 */
rtc_err_t pca2129_get_timestamp(rtc_t* rtc, real_time_t* curr_time) {
    curr_time->epoch = no_epoch; // Invalidate the epoch since we're about to update it

    // Note: timestamp does not have a weekdays register like the regular time register does.
    uint16_t rx_buffer[7] = {0}; // Time read is 7 bytes + 1 idle cycle due to command transmission
    uint16_t tx_buffer[7] = {RTC_READ | SUB_ADDRESS | TIMESTP_SEC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    mibspi_err_t err = MIBSPI_NO_ERR;
    rtc_err_t ret    = RTC_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(200))) {
        gioPORT_t* port       = rtc->cs_port;
        uint32_t pin          = rtc->cs_pin;
        mibspi_tg_t rtc_ts_tg = {.reg = RTC_TIME_MIBSPI_REG, .transfer_group = RTC_TIME_MIBSPI_GROUP, .cs_port = port, .cs_pin = pin};

        // Read the current time
        err = mibspi_transmit_receive(rtc_ts_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return RTC_MIBSPI_MUTEX_GRAB_ERR;
    }

    /* After the transmit completes, the read buffer should look as follows:
     * 		idle value | seconds | minutes | hours | days | months | years
     */
    curr_time->second = pca2129_bcd_to_bin(rx_buffer[1] & 0x7F);
    curr_time->minute = pca2129_bcd_to_bin(rx_buffer[2] & 0x7F);
    curr_time->hour   = pca2129_bcd_to_bin(rx_buffer[3] & 0x3F);
    curr_time->day    = pca2129_bcd_to_bin(rx_buffer[4] & 0x3F);
    curr_time->month  = pca2129_bcd_to_bin(rx_buffer[5] & 0x1F);
    curr_time->year   = pca2129_bcd_to_bin(rx_buffer[6]);

    // Set the generic MIBSPI error if one occurred
    ret = ((err != MIBSPI_NO_ERR) ? RTC_MIBSPI_ERR : RTC_NO_ERR);

    // Calculate the epoch if no MIBSPI errors occurred
    if (ret == RTC_NO_ERR) {
        ret = (set_real_time_epoch(curr_time) ? RTC_NO_ERR : RTC_EPOCH_INVALID);
    }

    rtc->most_recent_low_level_err = ret;
    return ret;
}

/**
 * @brief Set the current real time in the RTC.
 *
 * Set the current real time in the RTC.
 *
 * @param[inout] rtc The RTC to set the time on. Note: the prev_rtc_epoch field will be updated to
 * curr_time.
 * @param curr_time The time to set. The real time in the struct will be used, not the epoch field.
 * @return RTC_NO_ERR if successful, otherwise RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR
 */
rtc_err_t pca2129_set_current_time(rtc_t* rtc, real_time_t* curr_time) {
    mibspi_err_t err      = MIBSPI_NO_ERR;
    uint16_t tx_buffer[8] = {RTC_WRITE | SUB_ADDRESS | CLOCK_SECONDS,
                             pca2129_bin_to_bcd(curr_time->second),
                             pca2129_bin_to_bcd(curr_time->minute),
                             pca2129_bin_to_bcd(curr_time->hour),
                             pca2129_bin_to_bcd(curr_time->day),
                             0x00, // Weekdays
                             pca2129_bin_to_bcd(curr_time->month),
                             pca2129_bin_to_bcd(curr_time->year)};

    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(200))) {
        gioPORT_t* port       = rtc->cs_port;
        uint32_t pin          = rtc->cs_pin;
        mibspi_tg_t rtc_ts_tg = {.reg = RTC_TIME_MIBSPI_REG, .transfer_group = RTC_TIME_MIBSPI_GROUP, .cs_port = port, .cs_pin = pin};

        // Send command and time value to write
        err = mibspi_transmit(rtc_ts_tg, tx_buffer);

        /* Set the previous time in the RTC struct to the new set time so that incrementation
         * detection will still succeed.
         */
        set_real_time_epoch(curr_time); // Calculate epoch from the real time, in case it wasn't provided.
        rtc->prev_rtc_epoch = curr_time->epoch;

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return RTC_MIBSPI_MUTEX_GRAB_ERR;
    }

    rtc_err_t rtc_err              = ((err != MIBSPI_NO_ERR) ? RTC_MIBSPI_ERR : RTC_NO_ERR);
    rtc->most_recent_low_level_err = rtc_err;
    return rtc_err;
}

/**
 * pca2129_set_relative_alarm
 *
 * Sets an alarm within some seconds in the future.
 *
 * rtc:              which RTC to set alarm
 * seconds_to_alarm: within how many seconds should the alarm trigger
 * returns:          RTC_NO_ERR if successful, otherwise RTC_ALARM_*_INVALID or an RTC MIBSPI error.
 */
rtc_err_t pca2129_setup_relative_alarm(rtc_t* rtc, uint32_t seconds_to_alarm) {
    real_time_t alarm_time = orca_time_init;
    real_time_t curr_time  = orca_time_init;

    uint8_t days_in_month[12] = {0};
    if (is_leap_year(alarm_time.year)) {
        memcpy(days_in_month, days_in_month_leap, 12);
    } else {
        memcpy(days_in_month, days_in_month_no_leap, 12);
    }

    rtc_err_t err = pca2129_get_current_time(rtc, &curr_time);
    if (err != RTC_NO_ERR) {
        return err;
    }

    // Convert seconds to days/hours/minutes/seconds
    alarm_time.second = (uint8_t)(seconds_to_alarm % MAX_SECONDS) + curr_time.second;
    seconds_to_alarm /= MAX_SECONDS;

    alarm_time.minute = (uint8_t)(seconds_to_alarm % MAX_MINUTES) + curr_time.minute;
    seconds_to_alarm /= MAX_MINUTES;

    alarm_time.hour = (uint8_t)(seconds_to_alarm % MAX_HOURS) + curr_time.hour;
    seconds_to_alarm /= MAX_HOURS;

    uint32_t days    = seconds_to_alarm + curr_time.day;
    alarm_time.month = curr_time.month;
    alarm_time.year  = curr_time.year;

    // Adjust for overflow
    if (alarm_time.second > (MAX_SECONDS - 1)) {
        alarm_time.second -= MAX_SECONDS;
        alarm_time.minute++;
    }
    if (alarm_time.minute > (MAX_MINUTES - 1)) {
        alarm_time.minute -= MAX_MINUTES;
        alarm_time.hour++;
    }
    if (alarm_time.hour > (MAX_HOURS - 1)) {
        alarm_time.hour -= MAX_HOURS;
        days++;
    }
    if (days > days_in_month[alarm_time.month]) {
        alarm_time.day = days - days_in_month[alarm_time.month];
        alarm_time.month++;
    } else {
        alarm_time.day = days;
    }
    if (alarm_time.month > (MAX_MONTHS - 1)) {
        alarm_time.month -= MAX_MONTHS;
        alarm_time.year++;
    }

    return pca2129_setup_absolute_alarm(rtc, alarm_time);
}

/**
 * pca2129_set_absolute_alarm
 *
 * Sets an alarm at an absolute time in the future.
 *
 * rtc:        which RTC to set alarm
 * alarm_time: time to set the alarm to
 * returns:    RTC_NO_ERR if successful, otherwise RTC_ALARM_*_INVALID or an RTC MIBSPI error.
 */
rtc_err_t pca2129_setup_absolute_alarm(rtc_t* rtc, real_time_t alarm_time) {
    rtc_err_t err         = RTC_NO_ERR;
    real_time_t curr_time = {0};

    err = pca2129_get_current_time(rtc, &curr_time);
    if (err != RTC_NO_ERR) {
        return err;
    }

    rtc_err_t alarm_time_valid = pca2129_alarm_time_valid(curr_time, alarm_time);
    if (alarm_time_valid != RTC_ALARM_VALID) {
        return alarm_time_valid;
    }

    /* Write to alarm registers.
     *
     * The lower bits of alarm registers contain the value to compare to.
     * Bit 7 of each alarm register is an active-high flag indicating whether that register should
     * be used in the alarm time comparison.
     */
    err = pca2129_write_register(rtc, ALARM_DAYS, pca2129_bin_to_bcd(alarm_time.day | (1 << ALARM_EN_OFFSET)));
    if (err != RTC_NO_ERR) {
        return err;
    }
    err = pca2129_write_register(rtc, ALARM_HOURS, pca2129_bin_to_bcd(alarm_time.hour | (1 << ALARM_EN_OFFSET)));
    if (err != RTC_NO_ERR) {
        return err;
    }
    err = pca2129_write_register(rtc, ALARM_MINUTES, pca2129_bin_to_bcd(alarm_time.minute | (1 << ALARM_EN_OFFSET)));
    if (err != RTC_NO_ERR) {
        return err;
    }
    err = pca2129_write_register(rtc, ALARM_SECONDS, pca2129_bin_to_bcd(alarm_time.second | (1 << ALARM_EN_OFFSET)));

    rtc->most_recent_low_level_err = err;
    return err;
}

/**
 * @brief Enables an previously-configured alarm on an RTC.
 *
 * Absolute and relative alarms use the same backend features, so this function
 * is used to enable either alarm type.
 *
 * @param[in] rtc The RTC to enable the alarm on
 *
 * @return RTC_NO_ERR if everything succeeded, or RTC_MUTEX_GRAB_ERR/RTC_MIBSPI_ERR
 */
rtc_err_t pca2129_enable_alarm(rtc_t* rtc) {
    rtc_err_t err = RTC_NO_ERR;
    err           = pca2129_set_bit(rtc, CTRL_2, AF, 0);
    err           = pca2129_set_bit(rtc, CTRL_2, AIE, 1);

    rtc->most_recent_low_level_err = err;
    return err;
}

/*
 * pca2129_get_startup_status
 *
 * Get status of the RTC to detect if it has been restarted or not
 *
 * rtc: RTC device to check
 * returns: RTC_NO_ERR if successful, otherwise RTC_CLK_RESTARTED or an RTC MIBSPI error.
 */
rtc_err_t pca2129_get_startup_status(rtc_t* rtc) {
    rtc_err_t err     = RTC_NO_ERR;
    uint8_t osc_state = 0;

    err = pca2129_get_bit(rtc, CLOCK_SECONDS, OSC_STOP_FLAG, &osc_state);

    if (osc_state) {
        err = RTC_CLK_RESTARTED;
    }

    rtc->most_recent_low_level_err = err;
    return err;
}

/*
 * pca2129_handle_interrupts
 *
 * Deals with housekeeping following an interrupt.
 *
 * rtc:     RTC which caused the interrupt
 * returns: The source of the interrupt.
 */
rtc_irq_t pca2129_handle_interrupts(rtc_t* rtc) {
    uint8_t interrupt_status = 0;

    // Alarm Interrupt
    pca2129_get_bit(rtc, CTRL_2, AF, &interrupt_status);
    if (interrupt_status != 0) {
        pca2129_set_bit(rtc, CTRL_2, AF, 0);  // Clear the alarm flag
        pca2129_set_bit(rtc, CTRL_2, AIE, 0); // Remove alarm interrupt
        return ALARM_INT;
    }

    // Second Interrupt
    pca2129_get_bit(rtc, CTRL_2, MSF, &interrupt_status);
    if (interrupt_status != 0) {
        pca2129_set_bit(rtc, CTRL_2, MSF, 0);
        return SECOND_INT;
    }

    // Timestamp Interrupt
    pca2129_get_bit(rtc, CTRL_1, TSF1, &interrupt_status);
    if (interrupt_status != 0) {
        pca2129_set_bit(rtc, CTRL_1, TSF1, 0); // Clear flag

        rtc_err_t ts_status = RTC_NO_ERR;
        ts_status           = pca2129_get_bit(rtc, CTRL_2, TSF2, &interrupt_status);
        if (ts_status != 0) {
            pca2129_set_bit(rtc, CTRL_2, TSF2, 0);
            return TIMESTAMP_GROUND_INT; // Timestamp input driven to ground
        }
        return TIMESTAMP_EDGE_INT; // Timestamp rising or falling edge
    }

    // Battery Switch-Over Interrupt
    pca2129_get_bit(rtc, CTRL_3, BF, &interrupt_status);
    if (interrupt_status != 0) {
        pca2129_set_bit(rtc, CTRL_3, BF, 0); // Clear switch-over flag
        return SWITCH_OVER_BATTERY_INT;
    }

    // Battery Low Interrupt
    pca2129_get_bit(rtc, CTRL_3, BLIE, &interrupt_status);
    if (interrupt_status != 0) {
        // Flag cannot be cleared by command
        return BATTERY_LOW_INT;
    }

    return UNKNOWN_TRIGGER_INT;
}

/**
 * pca2129_bcd_to_bin
 *
 * Converts BCD to binary
 *
 * val:     BCD input
 * returns: val in binary
 */
static uint16_t pca2129_bcd_to_bin(uint8_t val) {
    return val - (6 * (val >> 4));
}

/**
 * pca2129_bin_to_bcd
 *
 * Converts binary to BCD
 *
 * val:     binary input
 * returns: val in BCD
 */
static uint8_t pca2129_bin_to_bcd(uint16_t val) {
    return val + (6 * (val / 10));
}

/**
 * @brief Sets the timestamp pin.
 * @param[in] val The value to set on the pin.
 */
void pca2129_timestamp_set(uint32_t val) {
    obc_gpio_write(RTC_TS_PORT, RTC_TS_PIN, val);
}

/* PRIVATE HELPER FUNCTIONS ---------------------------------------------------*/
/**
 * pca2129_alarm_time_valid
 *
 * Checks if a given alarm time exists and is in the future.
 *
 * curr_time:  The current time to compare with
 * alarm_time: Alarm time to validate
 * returns:    RTC_ALARM_VALID if successful, otherwise RTC_ALARM_*_INVALID
 */
static rtc_err_t pca2129_alarm_time_valid(real_time_t curr_time, real_time_t alarm_time) {
    uint8_t days_in_month[12] = {0};
    if (is_leap_year(alarm_time.year) == true) {
        memcpy(days_in_month, days_in_month_leap, 12);
    } else {
        memcpy(days_in_month, days_in_month_no_leap, 12);
    }

    // Check if valid date
    if ((alarm_time.day > (days_in_month[alarm_time.month] - 1)) || (alarm_time.hour > (MAX_HOURS - 1)) || (alarm_time.minute > (MAX_MINUTES - 1)) || (alarm_time.second > (MAX_SECONDS - 1))) {
        return RTC_ALARM_DATE_INVALID;
    }

    int8_t year_diff  = alarm_time.year - curr_time.year;
    int8_t month_diff = alarm_time.month - curr_time.month;

    // Alarm must be in the same year or a future year
    if (year_diff < 0) {
        return RTC_ALARM_YEAR_INVALID;
    }

    // Alarm cannot be more than one month in the future
    if ((month_diff > 1) || ((month_diff < 0) && (month_diff != -11))) {
        return RTC_ALARM_MONTH_INVALID;
    }

    // Alarm in same month and same year, must have a greater day/hour/minute/second
    if ((month_diff == 0) && (year_diff == 0)) {
        if (pca2129_compare_time_in_month(curr_time, alarm_time)) {
            return RTC_ALARM_VALID;
        } else {
            return RTC_ALARM_DAY_TIME_INVALID;
        }
    }

    // Alarm is next month but not more than a month from present day
    if (((month_diff == 1) && (year_diff == 0)) || ((month_diff == -11) && (year_diff == 1))) {
        if (pca2129_compare_time_in_month(curr_time, alarm_time)) {
            return RTC_ALARM_VALID;
        } else {
            return RTC_ALARM_DAY_TIME_INVALID;
        }
    }

    return RTC_ALARM_INVALID;
}

/**
 * pca2129_compare_time_in_month
 *
 * Checks if a given time is earlier in the month than a reference time.
 *
 * ref_time:        Reference time, should come earlier in the month
 * time_to_compare: Time to check, should come later in the month
 * returns:         1 if ref_time < time_to_compare, 0 otherwise
 */
static uint8_t pca2129_compare_time_in_month(real_time_t ref_time, real_time_t time_to_compare) {
    uint32_t ref_time_seconds = (((((ref_time.day * MAX_HOURS) + ref_time.hour) * MAX_MINUTES) + ref_time.minute) * MAX_SECONDS) + ref_time.second;

    uint32_t time_to_compare_seconds = (((((time_to_compare.day * MAX_HOURS) + time_to_compare.hour) * MAX_MINUTES) + time_to_compare.minute) * MAX_SECONDS) + time_to_compare.second;

    return (ref_time_seconds < time_to_compare_seconds);
}

/* REGISTER ACCESS FUNCTIONS -------------------------------------------------*/

/**
 * pca2129_set_bit
 *
 * Set a specific bit in a register
 *
 * rtc:        RTC device to write to
 * reg:        Register address in the RTC
 * bit_to_set: Offset in the register to set
 * val:        Bit value to write
 * returns:    RTC_NO_ERR if successful, RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR otherwise.
 */
static rtc_err_t pca2129_set_bit(rtc_t* rtc, uint8_t reg, uint8_t bit_to_set, uint8_t val) {
    // Read current register value
    uint8_t curr_reg = 0;
    rtc_err_t err    = pca2129_read_register(rtc, reg, &curr_reg);
    if (err != RTC_NO_ERR) {
        return err;
    }

    // Set appropriate bit
    if (val) {
        curr_reg |= (1 << bit_to_set);
    } else {
        curr_reg &= (uint8_t)(~(1 << bit_to_set));
    }

    // Write new register value
    err = pca2129_write_register(rtc, reg, curr_reg);
    return err;
}

/**
 * pca2129_get_bit
 *
 * Read a specific bit from a register
 *
 * rtc:        RTC device to read from
 * reg:        Register address in the RTC
 * bit_to_get: Offset in the register to read
 * val:        Pointer to where the read value should be stored
 * returns:    RTC_NO_ERR if successful, RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR otherwise.
 */
static rtc_err_t pca2129_get_bit(rtc_t* rtc, uint8_t reg, uint8_t bit_to_get, uint8_t* val) {
    rtc_err_t err = pca2129_read_register(rtc, reg, val);
    *val          = (*val >> bit_to_get) & 0x01;

    return err;
}

/**
 * pca2129_write_register
 *
 * Write to a single RTC register.
 *
 * rtc:     RTC device to read from
 * reg:     Register address in the RTC
 * val:     Value to write to the register
 * returns: RTC_NO_ERR if successful, RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR otherwise.
 */
static rtc_err_t pca2129_write_register(rtc_t* rtc, uint8_t reg, uint8_t val) {
    uint16_t tx_buffer[2] = {RTC_WRITE | SUB_ADDRESS | reg, val};

    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(200))) {
        gioPORT_t* port        = rtc->cs_port;
        uint32_t pin           = rtc->cs_pin;
        mibspi_tg_t rtc_reg_tg = {.reg = RTC_REG_MIBSPI_REG, .transfer_group = RTC_REG_MIBSPI_GROUP, .cs_port = port, .cs_pin = pin};

        // Send command and value to write to register
        err = mibspi_transmit(rtc_reg_tg, tx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return RTC_MIBSPI_MUTEX_GRAB_ERR;
    }

    return ((err != MIBSPI_NO_ERR) ? RTC_MIBSPI_ERR : RTC_NO_ERR);
}

/**
 * pca2129_read_register
 *
 * Read a single register from the RTC.
 *
 * rtc:      RTC device to read from
 * reg:      Register address in the RTC
 * curr_reg: Pointer to where the read value should be stored
 * returns:  RTC_NO_ERR if successful, RTC_MIBSPI_MUTEX_GRAB_ERR or RTC_MIBSPI_ERR otherwise.
 */
static rtc_err_t pca2129_read_register(rtc_t* rtc, uint8_t reg, uint8_t* curr_reg) {
    uint16_t tx_buffer[2] = {RTC_READ | SUB_ADDRESS | reg, 0xFF};
    uint16_t rx_buffer[2] = {0};

    mibspi_err_t err = MIBSPI_NO_ERR;
    if (xSemaphoreTake(xMibspiMutexHandle, pdMS_TO_TICKS(200))) {
        gioPORT_t* port        = rtc->cs_port;
        uint32_t pin           = rtc->cs_pin;
        mibspi_tg_t rtc_reg_tg = {.reg = RTC_REG_MIBSPI_REG, .transfer_group = RTC_REG_MIBSPI_GROUP, .cs_port = port, .cs_pin = pin};

        // Read register
        err = mibspi_transmit_receive(rtc_reg_tg, tx_buffer, rx_buffer);

        xSemaphoreGive(xMibspiMutexHandle);
    } else {
        return RTC_MIBSPI_MUTEX_GRAB_ERR;
    }

    *curr_reg = (uint8_t)rx_buffer[1];

    return ((err != MIBSPI_NO_ERR) ? RTC_MIBSPI_ERR : RTC_NO_ERR);
}

#endif
