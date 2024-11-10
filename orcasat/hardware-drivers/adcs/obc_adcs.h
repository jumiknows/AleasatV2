/**
 * @file obc_adcs.h
 * @brief CubeADCS module driver
 * @author Andres Martinez
 * @date Sep 14, 2020
 */

#ifndef OBC_ADCS_H
#define OBC_ADCS_H

#include "i2c_freertos.h"
#include "cube_util.h"

void adcs_create_infra(void);

adcs_err_t adcs_tlm(uint8_t tlm_id, uint8_t* tlm_data, uint16_t* tlm_len);
adcs_err_t adcs_tcm(uint8_t tcm_id, uint8_t* tcm_data, uint16_t tcm_len);

adcs_err_t adcs_ack(cube_tcm_ack_t* ack_status);
adcs_err_t adcs_comms(cube_comms_status_t* comms_status);

#endif
