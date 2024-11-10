/*
 * obc_temp.h
 *
 *  Created on: Feb 4, 2019
 *      Author: Melvin
 */

#ifndef OBC_TEMP_H_
#define OBC_TEMP_H_

#include "sys_common.h"
#include "obc_error.h"

temp_err_t read_temp(int16_t* temp);
void self_test_temperature(void);
int8_t obc_temp_ping(void);
void reset_temp(void);

#endif /* OBC_TEMP_H_ */
