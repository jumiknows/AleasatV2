#ifndef _RADIOTELEM_H
#define _RADIOTELEM_H
#include <stdint.h>
#include "adc.h"
#include "board_defaults.h"

typedef struct {
  uint16_t rt_pktctrl1;
  uint16_t rt_pktctrl0;
  uint16_t rt_mdmcfg4;
  uint16_t rt_mdmcfg3;
  uint16_t rt_mdmcfg2;
  uint16_t rt_mdmcfg1;
  uint16_t rt_mdmcfg0;
  uint16_t rt_deviatn;
  uint16_t rt_pa_table0;
  uint16_t rt_fsctrl1;
  uint16_t rt_fsctrl0;
  uint16_t rt_freq2;
  uint16_t rt_freq1;
  uint16_t rt_freq0;
  // rffe_t  rt_rffe;
  uint8_t rt_rffe;
} radiotelem_t;

extern __xdata radiotelem_t radiotelem;

void radiotelem_init(void);
void reset_radiotelem(void);
void set_radiotelem(radiotelem_t* rft);
void update_radiotelem(void);

#endif
