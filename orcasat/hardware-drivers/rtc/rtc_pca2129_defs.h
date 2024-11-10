/*
 * rtc_pca2129_defs.h
 *
 * Contains all register addresses and other defines
 * local to the RTC.
 *
 *  Created on: May 19, 2019
 * 		Author: Andrada Zoltan
 */

#ifndef RTC_PCA2129_DEFS_H_
#define RTC_PCA2129_DEFS_H_

/* UTILITY DEFINES */
#define MAX_SECONDS 60
#define MAX_MINUTES 60
#define MAX_HOURS   24
#define MAX_MONTHS  12

/* COMMAND MODES */
#define RTC_READ    (1 << 7)
#define RTC_WRITE   (0 << 7)
#define SUB_ADDRESS (1 << 5)

/* REGISTER ADDRESSES */
#define CTRL_1 0x00
#define CTRL_2 0x01
#define CTRL_3 0x02

#define CLOCK_SECONDS 0x03 // Coded in Binary Coded Decimal (BCD) Format, like abracon
#define CLOCK_MINUTES 0x04
#define CLOCK_HOURS   0x05
#define CLOCK_DAYS    0x06
#define CLOCK_WKDAYS  0x07
#define CLOCK_MONTHS  0x08
#define CLOCK_YEARS   0x09

#define ALARM_SECONDS 0x0A
#define ALARM_MINUTES 0x0B
#define ALARM_HOURS   0x0C
#define ALARM_DAYS    0x0D
#define ALARM_WKDAYS  0x0E

#define CLKOUT_CTL      0x0F // Temperature measurement period, OTP refresh, CLKOUT frequency
#define WATCHDG_TIM_CTL 0x10 // Enable/disable timer, interrupt pin signal, watchdog timer source clock frequency
#define WATCHDG_TIM_VAL 0x11 // Timer period

#define TIMESTP_CTRL 0x12
#define TIMESTP_SEC  0x13 // Second timestamp info, BCD format
#define TIMESTP_MIN  0x14 // Minutes timestamp info, BCD format
#define TIMESTP_HOUR 0x15 // AM or PM, hour timestamp info BCD (for 12 hr and 24 hr modes)
#define TIMESTP_DAY  0x16 // Day timestamp info, BCD format
#define TIMESTP_MON  0x17 // Month timestamp info, BCD format
#define TIMESTP_YEAR 0x18 // Year timestamp info, BCD format

#define AGING_OFFSET 0x19 // Correction ppm
#define ALARM_EN_OFFSET \
    0x07 // Offset for alarm enable bits. Alarm enable regs are [7] = Alarm enable (active high), \
         // [6:0] = BCD

/* REGISTER BIT OFFSETS */
// CTRL_1 register offsets
#define SI   0x00 // second interrupt
#define TSF1 0x04 // timestamp flag

// CTRL_2 register offsets
#define AIE  0x01 // alarm flag interrupt
#define TSIE 0x02 // enables/disables timestamp interrupt
#define AF   0x04 // alarm flag
#define TSF2 0x05 // other timestamp interrupt
#define MSF  0x07 // minute/second interrupt flag

// CTRL_3 register offsets
#define BLIE 0x00 // battery low interrupt enable
#define BIE  0x01 // battery (switch-over) interrupt enable
#define BLF  0x02 // battery low flag
#define BF   0x03 // battery (switch-over) flag
#define BTSE 0x04 // battery timestamp enable

// CLOCK_SECONDS register offsets
#define OSC_STOP_FLAG 0x07

// CLKOUT_CTL register offsets
#define COF_0 0x00 // CLKOUT frequency selection
#define COF_1 0x01
#define COF_2 0x02
#define OTPR  0x05 // OTPR: toggle to perform an OTP refresh (takes 100 ms)
#define TCR_0 0x06 // Temperature measurement period
#define TCR_1 0x07

// TIMESTP_CTL register offsets
#define TSOFF 0x06 // turns time stamp on or off

#endif /* RTC_PCA2129_DEFS_H_ */
