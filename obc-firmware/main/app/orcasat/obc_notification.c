/**
 * @file obc_notification.c
 * @brief Contains all ISR callbacks, redefined from the empty interrupt callbacks defined by
 * HALCoGEN.
 *
 * Making modifications here allows us to track our changes easily. When interrupts occur, the
 * low-level handling is done by lower-level HAL code. The HAL does things like clearing the
 * interrupt bits, and then calls the functions in this file. At this point, we take over and
 * process the interrupt information ourselves.
 *
 * Generally, when an interrupt occurs, we will push an item with information about the interrupt to
 * a FreeRTOS queue. This will cause functions to wake up and process the data.
 *
 * These functions execute in the interrupt context, so the FreeRTOS interrupt-safe API functions
 * must be used.
 *
 * @author Richard A
 * @date February 19, 2019
 */
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "tms_mibspi.h"
#include "obc_featuredefs.h"
#include "rtos.h"
#include "het.h"
#include "logger.h"
#include "gio.h"
#include "obc_serial_rx.h"
#include "gps_serial_rx.h"
#include "comms_mibspi.h"
#include "sci.h"

/**
 * @brief Callback for all TMS570 GIO interrupts.
 *
 * This function is called when a GIO pin triggers an interrupt. The port and pin can be checked to
 * determine which pin generated the interrupt. Handling of GIO interrupts is deferred to the gpio
 * driver, which will check for interrupts on the GPIO expander and the TMS570 itself. Interrupt
 * handling is accomplished with a task that wakes up when it receives items indicating the port and
 * pin - this function pushes those items to the queue so they can be processed and the interrupt
 * context exited.
 *
 * @warning this runs in interrupt context, so FreeRTOS interrupt-mode API functions must be used.
 *
 * @param port: the GIO port that triggered the interrupt
 * @param pin: the pin on the port that triggered the interrupt
 */
void gioNotification(gioPORT_t* port, uint32 bit) {
    gpio_irq_t irq_info = {.port = port, .pin = bit};
    service_gpio_irq(irq_info);
}

/**
 * @brief ISR callback for HET interrupts.
 * When HET pins are configured as GPIO with interrupt capability, this function will be called when
 * an interrupt occurs.
 * @pre edgeEnableNotification() has been called, which enables the edge interrupt on the pin.
 * @param hetREG: pointer to the HET peripheral that generated the interrupt. hetPORTx
 * @param edge: the edge number that the interrupt occurred on - not necessarily the pin, check
 * configuration in HALCoGen.
 * @warning this runs in interrupt context, so FreeRTOS interrupt-mode API functions must be used.
 */
void edgeNotification(hetBASE_t* hetREG, uint32 edge) {
    /* Edge notification example
     * 	- see examples/het_notification_example.c
     */
    if ((hetREG == COMMS_INT_REG) && (edge == COMMS_INT_EDGE)) {
        // Notify Comms service task to read message from SPI
        comms_mibspi_invoke_irq_cb();
    }
}

/**
 * @brief ISR callback from the SCI peripheral (UART)
 * @param flags: interrupt status flags (see sci.h for more info)
 * @param sci: pointer to SCI peripheral that the interrupt was raised by.
 * @warning this runs in interrupt context, so FreeRTOS interrupt-mode API functions must be used.
 */
void sciNotification(sciBASE_t* sci, uint32 flags) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((flags & SCI_RX_INT) == SCI_RX_INT) {
        if (sci == UART_DEBUG) {
            obc_serial_rx_isr(&xHigherPriorityTaskWoken);
        } else if (sci == UART_GPS) {
            gps_serial_rx_isr(&xHigherPriorityTaskWoken);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief MIBSPI interrupt callback
 *
 * This function is executed when the MIBSPI peripheral transfer is complete.
 * Using the mibSPI event group, this callback wakes up waiting threads by
 * setting the bit of the respective transfer group.
 * @param mibspi: pointer to the MIBSPI port that raised the interrupt
 * @param group: transfer group that the interrupt is associated with
 * @warning this runs in interrupt context, so FreeRTOS interrupt-mode API functions must be used.
 */
void mibspiGroupNotification(mibspiBASE_t* mibspi, uint32 group) {
    /* xHigherPriorityTaskWoken must be initialized to pdFALSE. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    EventGroupHandle_t eg = get_eventgroup_handle(mibspi);
    // TODO ALEA-774 Should we have a log_from_isr function to log if this returns NULL?

    xEventGroupSetBitsFromISR(eg,                         /* The event group being updated. */
                              (EventBits_t)(1 << group),  /* The bits being set. */
                              &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief MIBSPI error callback
 *
 * This is a error callback that is provided by the application and is called upon
 * an error interrupt. Using the mibSPI event group, this callback wakes up waiting
 * threads by setting the MIBSPI_ERR_NOTIF bit.
 *
 * @param mibspi:  pointer to the MIBSPI port that raised the interrupt
 * @param flags:   Copy of error interrupt flags
 * @warning this runs in interrupt context, so FreeRTOS interrupt-mode API functions must be used.
 */
void mibspiNotification(mibspiBASE_t* mibspi, uint32 flags) {
    /* xHigherPriorityTaskWoken must be initialized to pdFALSE. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    EventGroupHandle_t eg = get_eventgroup_handle(mibspi);
    // TODO ALEA-774 Should we have a log_from_isr function to log if this returns NULL?

    xEventGroupSetBitsFromISR(eg,                             /* The event group being updated. */
                              (EventBits_t)MIBSPI_ERR_NOTIF,  /* The bits being set. */
                              &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
