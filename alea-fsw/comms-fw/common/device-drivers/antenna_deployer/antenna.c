#include <cc1110.h>
#include "cc1110_regs.h"
#include "board_defaults.h"

void handle_antenna_deployment(void){
  if((ANT_DEPLOY_DISABLE & alea_rffe) == ANT_DEPLOY_DISABLE){
    P0_3 = ANT_DEPLOY_DISABLE;
  }
  else {
    P0_3 = ANT_DEPLOY_ENABLE;
  }
  return;
}
