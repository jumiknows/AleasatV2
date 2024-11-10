/**
 * @file file_downlink.h
 * @brief File downlink protocol implementation based on the CFDP constructs.
 *
 * @author Andrada Zoltan, Alireza Alidousti
 * @date April 2, 2020
 */

#ifndef FILE_DOWNLINK_H_
#define FILE_DOWNLINK_H_

#include "obc_error.h"

fs_err_t simple_file_downlink(const char* path);

#endif /* FILE_DOWNLINK_H_ */
