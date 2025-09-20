/**
 * @file obc_gpio.c
 * @brief OBC GPIO abstraction to handle GPIO expander and internal TMS570 GPIOs with the same API.
 *
 * Generally, GIO refers only to the GIO ports/pins on the TMS570. GPIO is used as a more generic
 * term, to encompass GPIO expander pins or internal GIO pins.
 *
 * Since the OBC has internal GIO pins and the GPIO expander, these functions allow both to be dealt
 * with in the same way. GIO pins support interrupts (must be configured in HALCoGEN), and GPIO
 * expander pins support interrupts too.
 *
 * In this file, several things are set up. These features require user modification, follow the
 * instructions in comments tagged with "MODIFY HERE."
 *  - GIO interrupts are turned on.
 *  - GPIO expander pins are set up (input/output/interrupt).
 *  - GIO interrupts are handled.
 *
 * Turning on GIO interrupts:
 *  - Insert calls to gioEnableNotification() in @ref obc_gpio_init().
 *
 * Setting up GPIO expander pins:
 *  - Also see the GPIO expander header, especially if using interrupt capabilities.
 *
 * Handling GIO interrupts:
 *  - See instructions in @ref vGPIOServiceTask().
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_gpio.h"

// OBC
#include "obc_gpio_err.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_rtos.h"
#include "obc_watchdog.h"
#include "tms_can.h"

#include "obc_rtc.h"
#include "gpio_pcal6416a.h"

// Logging
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "gio.h"
#include "het.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Number of interrupt service requests for GPIOs to queue.
 *
 * If @ref xGPIOInterruptTaskHandle has a low priority, this queue should be increased in size.
 */
#define GIO_IRQ_Q_NUM_ITEMS 2

/**
 * @brief Size of the items within the interrupt queue
 */
#define GIO_IRQ_Q_ITEM_SIZE sizeof(gpio_irq_t)

#define GIO_IRQ_POLL_PERIOD_MS 1000U

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Setup for the queue that receives interrupt information
static QueueHandle_t gioInterruptQueue = NULL;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void vGPIOServiceTask(void *pvParameters);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize internal and expander GPIO pins.
 * @pre: gioInit() from the HAL has been called.
 * @pre: obc_i2c_init() has been called, which initializes the FreeRTOS I2C subsystem and therefore
 * allows the GPIO expander to be configured and used.
 * @pre: OBC watchdog system has been initialized.
 */
void gpio_init_hw(void) {
    /* Initialize TMS570 GPIO */
    gioInit();
}

/**
 * @brief Enable GPIO interrupts.
 */
void gpio_init_irq(void) {
#if FEATURE_GPIO_EXPANDER
    gioEnableNotification(OBC_EXPAND_IRQ_N_PORT, OBC_EXPAND_IRQ_N_PIN);
#endif
#if FEATURE_HW_RTC
    gioEnableNotification(RTC_ALARM_IRQ_N_PORT, RTC_ALARM_IRQ_N_PIN);
#endif
    /* MODIFY HERE: add further interrupt enable calls, if required */
}

/**
 * @brief Initialize RTOS features required for GPIO interrupt capability.
 *
 * A queue is used to push items with the interrupt details (port and pin) to a task @ref
 * vGPIOServiceTask() that will process them. Queue pushes are done by the GIO interrupt callback in
 * @ref obc_notification.c. This function creates the queue and the interrupt servicing task.
 */
void gpio_pre_init(void) {
    static StaticQueue_t xIRQStaticQueue;
    static uint8_t gio_irq_q_storage[GIO_IRQ_Q_NUM_ITEMS * GIO_IRQ_Q_ITEM_SIZE];

    gioInterruptQueue = xQueueCreateStatic(GIO_IRQ_Q_NUM_ITEMS, GIO_IRQ_Q_ITEM_SIZE, gio_irq_q_storage, &xIRQStaticQueue);

    obc_rtos_create_task(OBC_TASK_ID_GPIO_IRQ, &vGPIOServiceTask, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Enable interrupts in post-init section
 */
void gpio_post_init(void) {
    gpio_init_irq();
}

/**
 * @brief: initialize expander GPIO pins.
 * @pre: obc_i2c_init() has been called, which initializes the FreeRTOS I2C subsystem.
 *
 * When adding a pin controlled by the GPIO expander, it must be configured in this function.
 * Use the blinky pin as an example, and @ref gpio_pcal6416a.h functions.
 */
void gpio_expander_init(void) {
#if FEATURE_GPIO_EXPANDER

    /* Initialize the expander with reset values */
    if (pcal6416a_init() != GPIO_SUCCESS) {
        LOG_GPIO_EXPANDER__INIT_FAILED();
    }

    /* Configure the desired pins */
    if (pcal6416a_configure_output(EXPANDER_BLINKY_PORT.reg.exp, EXPANDER_BLINKY_PIN, 1, PULLUP) != GPIO_SUCCESS) {
        LOG_GPIO_EXPANDER__BLINK_INIT_FAILED();
    }

    /* MODIFY HERE: add further pcal6416a_configure_output(), pcal6416a_configure_input(), pcal6416a_configure_interrupt()
     * calls */

    /* END MODIFIABLE REGION */

    if (pcal6416a_validate_regs() != GPIO_SUCCESS) {
        LOG_GPIO_EXPANDER__VERIFY_FAILED();
    }

#endif
}

/**
 * @brief Reset of GPIO expander
 */
void gpio_expander_reset(void) {
#if FEATURE_GPIO_EXPANDER
    pcal6416a_reset();
#endif
}

/**
 * @brief GPIO IRQ queue submission task. Called when interrupts are detected on an
 * interrupt-capable pin.
 *
 * Submits interrupt information on a queue so that it can be serviced by the GPIO interrupt task.
 *
 * @pre GPIO initialization has been performed, obc_i2c_init has been performed if using GPIO
 * expander interrupts.
 * @param irq_info: Port and pin information from the notification.
 */
void service_gpio_irq(gpio_irq_t irq_info) {
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    if (gioInterruptQueue != NULL) {
        if ((xQueueSendToBackFromISR(gioInterruptQueue, (void *)&irq_info, &xHigherPriorityTaskWoken)) == errQUEUE_FULL) {
            // TODO ALEA-774 Should we have a log_from_isr function?
            //log_str(ERROR, LOG_GPIO, "IRQ queue full");
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Write a value to a GPIO pin.
 *
 * Common function for writing GPIO pins, regardless of whether they are on the TMS570 or on the
 * GPIO expander.
 *
 * @param[in] port  GPIO port to use (use GIO_PORT, EXP_PORT or CAN_PORT macros)
 * @param[in] pin   The pin on the respective port.
 * @param[in] value 0 or 1, the value to write to the pin.
 * @return GPIO_SUCCESS for all internal GPIO writes, and if the expander write did not experience
 * any IO errors. GPIO_FAILURE if the port type was invalid or I2C communication with the expander failed.
 */
gpio_err_t obc_gpio_write(gpio_port_t port, uint32_t pin, uint32_t value) {
    gpio_err_t err;

    switch (port.type) {
    case GPIO_PORT_GIO:
        gioSetBit(port.reg.gio, pin, value);
        err = GPIO_SUCCESS;
        break;

    case GPIO_PORT_CAN:
        tms_can_gpio_write(port.reg.can, (can_pin_t)pin, value);
        err = GPIO_SUCCESS;
        break;
#if FEATURE_GPIO_EXPANDER

    case GPIO_PORT_EXP:
        err = pcal6416a_gpio_write(port.reg.exp, pin, value);
        break;
#endif

    default:
        err = GPIO_FAILURE;
        break;
    }

    return err;
}

/**
 * @brief Read a value from a GPIO pin.
 *
 * Common function for reading GPIO pins, regardless of whether they are on the TMS570 or on the
 * GPIO expander.
 *
 * @param[in]  port  GPIO port to use (use GIO_PORT, EXP_PORT or CAN_PORT macros)
 * @param[in]  pin   The pin on the respective port.
 * @param[out] value Provide a uint32_t to store the read value (0 = LOW, 1 = HIGH)
 * @return GPIO_SUCCESS for all internal GPIO reads, and if the expander read did not experience
 * any IO errors. GPIO_FAILURE if the port type was invalid or I2C communication with the expander failed.
 */
gpio_err_t obc_gpio_read(gpio_port_t port, uint32_t pin, uint32_t *value) {
    gpio_err_t err;

    switch (port.type) {
    case GPIO_PORT_GIO:
        *value = gioGetBit(port.reg.gio, pin);
        err = GPIO_SUCCESS;
        break;

    case GPIO_PORT_CAN:
        *value = tms_can_gpio_read(port.reg.can, (can_pin_t)pin);
        err = GPIO_SUCCESS;
        break;
#if FEATURE_GPIO_EXPANDER

    case GPIO_PORT_EXP:
        err = pcal6416a_gpio_read(port.reg.exp, pin, value);
        break;
#endif

    default:
        err = GPIO_FAILURE;
        break;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief GPIO interrupt handler task.
 *
 * This task wakes up when it receives queue pushes from the GPIO notification handler. It checks
 * the interrupt request details and calls the appropriate user-defined function. Calling the
 * appropriate function for the port/pin that raised the interrupt is done here.
 *
 * User modification: check the port and pin that come in with @ref irq_info, call appropriate
 * function.
 *
 * @param pvParameters: used implicitly for watchdog handling.
 */
static void vGPIOServiceTask(void *pvParameters) {
    gpio_irq_t irq_info  = {.port = NULL, .pin = 0};

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_GPIO_IRQ);

        if ((xQueueReceive(gioInterruptQueue, &(irq_info), pdMS_TO_TICKS(GIO_IRQ_POLL_PERIOD_MS))) == pdPASS) {

            /* Handle pins on the GPIO expander if it exists */
#if FEATURE_GPIO_EXPANDER
            if ((irq_info.port == OBC_EXPAND_IRQ_N_PORT) && (irq_info.pin == OBC_EXPAND_IRQ_N_PIN)) {
                pcal6416a_handle_interrupts();
            }

            /* MODIFY HERE: handle other GPIO expander pins */
            // Your code here
            /* END MODIFIABLE REGION */
#endif

            /* MODIFY HERE: handle other pins - check the port and pin and call appropriate
             * functions. Keep in mind the priority of this task. */
            // Your code here

#if FEATURE_HW_RTC

            if ((irq_info.port == RTC_ALARM_IRQ_N_PORT) && (irq_info.pin == RTC_ALARM_IRQ_N_PIN)) {
                rtc_alarm_isr();
            }

#endif
            /* END MODIFIABLE REGION */

        }
    }
}
