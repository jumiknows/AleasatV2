#include "radiotelem.h"
#include "radio.h"
#include "stringx.h"
#include "board_defaults.h"

__xdata radiotelem_t radiotelem;
volatile uint8_t rffe = 15;

void radiotelem_init(void) {
    memsetx((__xdata void*) &radiotelem, 0, sizeof(radiotelem));
}

void reset_radiotelem(void){
 return;
}

void set_radiotelem(radiotelem_t* rft){
  PKTCTRL1=rft->rt_pktctrl1;
  PKTCTRL0=rft->rt_pktctrl0;
  MDMCFG4=rft->rt_mdmcfg4;
  MDMCFG3=rft->rt_mdmcfg3;
  MDMCFG2=rft->rt_mdmcfg2;
  MDMCFG1=rft->rt_mdmcfg1;
  MDMCFG0=rft->rt_mdmcfg0;
  DEVIATN=rft->rt_deviatn;
  PA_TABLE0=rft->rt_pa_table0;
  FSCTRL1=rft->rt_fsctrl1;
  FSCTRL0=rft->rt_fsctrl0;
  FREQ2=rft->rt_freq2;
  FREQ1=rft->rt_freq1;
  FREQ0=rft->rt_freq0;
  // alea_rffe.antenna_deploy=rft->rt_rffe.antenna_deploy;
  // alea_rffe.radio_pa_ctrl=rft->rt_rffe.radio_pa_ctrl;
  // alea_rffe.antenna_sel=rft->rt_rffe.antenna_sel;
  // alea_rffe.radio_path_sel=rft->rt_rffe.radio_path_sel;
    alea_rffe=rft->rt_rffe;
}

void update_radiotelem(void) {
  radiotelem.rt_pktctrl1=PKTCTRL1;
  radiotelem.rt_pktctrl0=PKTCTRL0;
  radiotelem.rt_mdmcfg4=MDMCFG4;
  radiotelem.rt_mdmcfg3=MDMCFG3;
  radiotelem.rt_mdmcfg2=MDMCFG2;
  radiotelem.rt_mdmcfg1=MDMCFG1;
  radiotelem.rt_mdmcfg0=MDMCFG0;
  radiotelem.rt_deviatn=DEVIATN;
  radiotelem.rt_pa_table0=PA_TABLE0;
  radiotelem.rt_fsctrl1=FSCTRL1;
  radiotelem.rt_fsctrl0=FSCTRL0;
  radiotelem.rt_freq2=FREQ2;
  radiotelem.rt_freq1=FREQ1;
  radiotelem.rt_freq0=FREQ0;
  // radiotelem.rt_rffe.antenna_deploy=alea_rffe.antenna_deploy;
  // radiotelem.rt_rffe.radio_pa_ctrl=alea_rffe.radio_pa_ctrl;
  // radiotelem.rt_rffe.antenna_sel=alea_rffe.antenna_sel;
  // radiotelem.rt_rffe.radio_path_sel=alea_rffe.radio_path_sel;
    radiotelem.rt_rffe=alea_rffe;
}
