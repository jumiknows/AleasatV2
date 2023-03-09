/*
 * low_power.c
 *
 *  Created on: Nov 4, 2018
 *      Author: Richard
 */

#include "low_power.h"
#include "sys_common.h"
#include "obc_featuredefs.h"
#include "system.h"
#include "stdbool.h"

volatile uint8_t enable_sleep;

#define OBC_IDLE_SLEEP_1 0x00021B32
/*	                          21B32
 *   						   |  |
 *                             |  }-------------- CSDISSET: 32 will disable PLL1, LPOs. OSCIN
 *enabled (clock source disable set, 1 = disabled)
 *                             }----------------- CDDIS: 21B will disable GCLK, HCLK, VCLK2, VCLKA1,
 *VCLK4. RTICLK and peripheral VCLK remain enabled. (clock domain disable)
 *
 *	Note: this assumes the define is used with the systemPowerDown Function.
 */

#define OBC_IDLE_SLEEP_2 0x00021B02
/*	                          21B02
 *   						   |  |
 *                             |  }-------------- CSDISSET: 32 will disable PLL1. OSCIN and LPOs are
 *enabled still (clock source disable set, 1 = disabled)
 *                             }----------------- CDDIS: 21B will disable GCLK, HCLK, VCLK2, VCLKA1,
 *VCLK4. RTICLK and peripheral VCLK remain enabled. (clock domain disable)
 *
 *	Note: this assumes the define is used with the default systemPowerDown Function.
 */
static void asm_dsb(void);

void idle_sleep_on(void) {
    enable_sleep = 1;
}

void idle_sleep_off(void) {
    enable_sleep = 0;
}

void enter_idle_sleep(void) {
    if (enable_sleep == 1) {
        asm_dsb();
        systemPowerDown(OBC_IDLE_SLEEP_2);
    }
}

static void asm_dsb(void) {
    asm(" DSB");
}
