#ifndef _COMMS_MIBSPI_H_
#define _COMMS_MIBSPI_H_

#include "comms_device.h"
#include "rtos.h"
#include "event_groups.h"

comms_dev_handle_t comms_mibspi_get_handle(void);
void comms_mibspi_init(void);
void comms_mibspi_invoke_irq_cb(void);

#endif /* _COMMS_MIBSPI_H_ */
