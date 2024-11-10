/**
 * @file gpio_pcal6416a_defs.h
 * @brief Register addresses for the PCAL6416A GPIO expander.
 * @date Aug 31, 2019
 * @author Kabir B
 */

#ifndef GPIO_PCAL6416A_DEFS_H_
#define GPIO_PCAL6416A_DEFS_H_

#include "obc_featuredefs.h"

#if GPIO_EXPANDER_EN

// REGISTER ADDRESSES
#define INPUT_PORT_0       0x00
#define INPUT_PORT_1       0x01
#define OUTPUT_PORT_0      0x02
#define OUTPUT_PORT_1      0x03
#define POL_INV_PORT_0     0x04
#define POL_INV_PORT_1     0x05
#define CONFIG_PORT_0      0x06
#define CONFIG_PORT_1      0x07
#define OUTPUT_DRV_STR_0   0x40
#define OUTPUT_DRV_STR_A0  0x41
#define OUTPUT_DRV_STR_1   0x42
#define OUTPUT_DRV_STR_A1  0x43
#define INPUT_LATCH_REG_0  0x44
#define INPUT_LATCH_REG_1  0x45
#define PULL_UP_DOWN_ENA_0 0x46
#define PULL_UP_DOWN_ENA_1 0x47
#define PULL_UP_DOWN_SEL_0 0x48
#define PULL_UP_DOWN_SEL_1 0x49
#define IRQ_MASK_0         0x4A
#define IRQ_MASK_1         0x4B
#define IRQ_STATUS_0       0x4C
#define IRQ_STATUS_1       0x4D
#define OUTPUT_PORT_CONFIG 0x4F

#endif /* GPIO_EXPANDER_EN true */
#endif /* GPIO_PCAL6416A_DEFS_H_ */
