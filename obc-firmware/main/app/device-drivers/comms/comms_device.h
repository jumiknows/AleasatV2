/**
 * @file comms_device.h
 *
 * @brief External API for the comms device driver interface.
 */
#ifndef COMMS_DEVICE_H
#define COMMS_DEVICE_H

#include <stdint.h>

typedef enum comms_dev_err_type {
    COMMS_DEV_SUCCESS = 0,
    COMMS_DEV_ERR_BUS_FAILURE = -1,
    COMMS_DEV_ERR_INVALID_ARG = -2
} comms_dev_err_type_t;

typedef void* comms_dev_handle_t;
typedef void (*comms_dev_cb_func_t)(void* param);

typedef struct {
    comms_dev_err_type_t (*tx)(uint16_t* buffer, uint16_t num_bytes);
    comms_dev_err_type_t (*rx)(uint16_t* buffer, uint16_t buffer_len);
    comms_dev_cb_func_t cb; // (c)all(b)ack function to notify upper layers of incoming data
    void* cb_param;
} comms_device_t;

comms_dev_err_type_t comms_dev_send(
    comms_dev_handle_t cdev_hdl,
    uint16_t* buffer,
    uint16_t num_bytes 
);

comms_dev_err_type_t comms_dev_receive(
    comms_dev_handle_t cdev_hdl,
    uint16_t* buffer,
    uint16_t buffer_len
);

void comms_dev_register_callback(
    comms_dev_handle_t cdev_hdl,
    comms_dev_cb_func_t cb,
    void* cb_param
);

#endif
