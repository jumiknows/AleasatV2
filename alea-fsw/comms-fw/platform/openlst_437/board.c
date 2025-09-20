// OpenLST
// Copyright (C) 2018 Planet Labs Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cc1110.h>
#include "cc1110_regs.h"
#include "board_defaults.h"

void board_init(void) {

    // ALEA OBC SPI SS Request, triggers on falling edge
    P0SEL &= ~(1<<0);  // GPIO not peripheral
    P0DIR |= 1<<0;       // Output not input
    P0_0 = 1;                  // Initialize as 1

    // Watchdog Initialize
    P0SEL &= ~(1<<1);  // GPIO not peripheral
    P0DIR |= 1<<1;       // Output not input
    P0_1 = 0;                  // <>

    // Antenna deployment Signal
    P0SEL &= ~(1<<3);  // GPIO not peripheral
    P0DIR |= 1<<3;       // Output not input
    P0_3 = 0;                  // Initialize as not active

    // EPS nRST
    P1SEL &= ~(1<<0);  // GPIO not peripheral
    P1DIR |= 1<<0;       // Output not input
    P1_0 = 1;                     // Initialize not active

    // OBC nRST
    P1SEL &= ~(1<<1);  // GPIO not peripheral
    P1DIR |= 1<<1;       // Output not input
    P1_1 = 1;                     // Initialize not active

    // Antenna Select
    P1SEL &= ~(1<<6);  // GPIO not peripheral
    P1DIR |= 1<<6;       // Output not input
    P1_6 = 0;                     // <>

    // Path Control
    P1SEL &= ~(1<<7);  // GPIO not peripheral
    P1DIR |= 1<<7;       // Output not input
    P1_7 = 0;                     // <>

    // Power Amplifier control
    P2SEL &= ~(1<<0);  // GPIO not peripheral
    P2DIR |= 1<<0;       // Output not input
    P2_0 = 0;                     // <>

    /* Set up radio "test" signal pins.  We're setting these up to be
       outputs from the radio core indicating when the radio is
       off-board RF path switches.  Once set up, they are automatically
       toggled appropriately by the radio core whenever the radio state
       changes. */
    // Disbled automatic toggle. Will do manually
  IOCFG2 = IOCFG2_GDO2_INV_ACTIVE_HIGH | IOCFG_GDO_CFG_NONE;
  //Disbled automatic toggle. Will do manually
  IOCFG1 = IOCFG1_GDO1_INV_ACTIVE_LOW | IOCFG_GDO_CFG_NONE;
  // No special function for P1_5 (IOCFG0)
}


void board_led_set(__bit led_on) {
  P0_7 = led_on;
}

void board_pre_tx(void){

    //select antenna
    if((SELECT_ANT_SOLAR & alea_rffe) == SELECT_ANT_SOLAR){
    P1_6 = SELECT_ANT_SOLAR;
  }
  else {
    P1_6 = SELECT_ANT_HEADER;
  }

    //select path
    if((SELECT_PA_RF_PATH & alea_rffe) == SELECT_PA_RF_PATH){
    P1_7 = SELECT_PA_RF_PATH;
  }
  else {
    P1_7 = SELECT_TRX_RF_PATH;
  }

    //enable pa if selected
    if((PA_ENABLE & alea_rffe) == PA_ENABLE){
    P2_0 = PA_ENABLE;
  }
  else {
    P2_0 = PA_DISABLE;
  }

    return;
}

void board_pre_rx(void){
        // disbale pa
        P1_7 = PA_DISABLE;
        // select receiving path
        P2_0 = SELECT_TRX_RF_PATH;

        // select antenna
        if((SELECT_ANT_SOLAR & alea_rffe) == SELECT_ANT_SOLAR){
        P1_6 = SELECT_ANT_SOLAR;
      }
      else {
        P1_6 = SELECT_ANT_HEADER;
      }

    return;
}
