/**
 * @file obc_main.c
 * @brief Entry point for the application and the main FreeRTOS task
 * that creates all other tasks.
 * @author Richard Arthurs
 * @date April 18, 2020
 */
#include "obc_main.h"
#include "rtos.h"
#include "sys_core.h"
#include "rti.h"
#include "het.h"
#include "low_power.h"
#include "obc_rtos.h"
#include "obc_featuredefs.h"
#include "obc_rtc.h"
#include "tms_mibspi.h"
#include "tms_mibspi.h"
#include "obc_startup.h"
#include "obc_flash.h"
#include "obc_watchdog.h"
#include "obc_gpio.h"
#include "blinky.h"
#include "obc_serial_rx.h"
#include "obc_serial_tx.h"
#include "log_sys.h"
#include "logger.h"
#include "obc_filesystem.h"
#include "obc_settings.h"
#include "obc_adc.h"
#include "obc_mram.h"

#include "comms_api.h"
#include "comms_defs.h"

#if COMMS_OVER_SERIAL
#include "comms_obc_serial.h"
#else
#include "comms_mibspi.h"
#endif

#include "tms_can.h"
#include "tms_spi.h"
#include "tms_i2c.h"
#include "cmd_sys_imm.h"
#include "cmd_sys_sched.h"
#include "cmd_sys_exec.h"
#include "gndstn_link.h"
#include "gps_serial_rx.h"
#include "obc_gps.h"

// Private Functions
static void obc_main_task(void* pvParameters);

/**
 * @brief The main task for the application.
 *
 * This task creates all of the other tasks after initializing hardware and
 * various other RTOS infrastructure, such as queues.
 *
 * @note This task should be created with portPRIVILEGE_BIT because some of
 * the hardware initialization requires privileged mode.
 *
 * @param pvParameters is not used.
 */
static void obc_main_task(void* pvParameters) {
    // Create FreeRTOS mutexes and queues for all features. Doing this allows future startup steps
    // to do things like push to the UART TX queue, which is done when logging errors.
    // None of this code relies on other application features.
    obc_watchdog_create_infra();
    obc_serial_rx_create_infra();
    obc_serial_tx_create_infra();
    log_sys_create_infra();
    gps_serial_rx_create_infra();
    tms_i2c_create_infra();
    gpio_create_infra();
    tms_mibspi_create_infra();
    rtc_create_infra();
    // telem_create_infra();
    gps_create_infra();
    tms_spi_create_infra();
    rtc_scheduler_create_infra();
    cmd_sys_exec_create_infra();
    cmd_sys_sched_create_infra();
    gndstn_link_create_infra();

    // Start the backup epoch so we have a timestamp before initializing the hardware RTCs.
    // If errors occur in subsequent steps, they will be able to properly log the error because
    // the backup epoch is providing a time reference.
    rtc_init_backup();

    // Initialize low-level hardware for all features, but do not enable interrupts for most
    // features yet. Interrupts cannot be safely enabled until the tasks that deal with handling the
    // interrupts have been created. Hardware features where interrupts are enabled during
    // init are tagged with "_irq" in the init function name.
    tms_i2c_init();
    gpio_init_hw();
    tms_mibspi_init_hw();
    comms_service_create_infra();
#if COMMS_OVER_SERIAL
    comms_obc_serial_create_infra();
#else
    comms_mibspi_init();
#endif
    hetInit();
    adc_init();
    tms_can_init();
    tms_spi_init();

    // Start the MRAM and load settings from NVCT. This requires that MIBSPI is operational first.
    // On LaunchPad, provision the NVCT now because it doesn't have a non-volatile NVCT, so we
    // would otherwise need to provision manually after each reset. The NVCT on OBCs needs to be
    // provisioned manually, but it's non-volatile.
    init_mram();
#ifdef PLATFORM_LAUNCHPAD_1224
    provision_new_settings_table(0);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO after dealing with MRAM (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/57)
    provision_new_settings_table(0);
#endif
    update_settings_from_nvct();

    // Start the tasks that deal with hardware. These tasks process requests from queues, and also
    // handle interrupts. These tasks must be created before the interrupts are enabled, because
    // interrupt processing requires that these tasks can run.
    cmd_sys_exec_start_task();
    cmd_sys_imm_start_task();
    cmd_sys_sched_start_task();
    obc_serial_rx_create_task();
    log_sys_create_task();
    gps_serial_rx_start_task();
    gpio_start_task();

#if COMMS_OVER_SERIAL
    comms_obc_serial_create_task();
    comms_dev_handle_t cdev = comms_obc_serial_get_handle();
#else
    comms_dev_handle_t cdev = comms_mibspi_get_handle();
#endif
    comms_mngr_start_task(cdev);

    gndstn_link_start_task();

    // Start the GPIO blinky task. This is a useful indicator because if anything is really wrong
    // with startup, it might stop blinking.
    blinky_start_task();

    // Initialize the complex external hardware.
    // The RTC requires MIBSPI to be working before it can be initialized.
    // The GPIO expander requires I2C to be working before it can be initialized.
    rtc_init();

    gpio_expander_init();

    // Enable interrupts. This is safe to do now that the interrupt processing tasks have been
    // started.
    gpio_init_irq();

    // Enable SciDriver. The GPS and Serial drivers require this to be called before their setup functions.
    sciInit();

    // Enable interrupts for the UART. This should only be done after the scheduler is started,
    // because the scheduler processes commands received from the UART.
    obc_serial_rx_init_irq();
    gps_serial_rx_init_irq();

    // Filesystem initialization requires flash, and takes a couple of seconds because we erase it
    // at startup.
    flash_init();
    fs_init();

    gps_init();

    // Hardware is ready to go now. Print out some information about startup.
    LOG_PRINT_GENERAL__STARTUP_COMPLETE();
#if defined(PLATFORM_ALEA_V1)
    LOG_HW_TYPE__ALEA_V1();
#elif defined(PLATFORM_LAUNCHPAD_1224)
    LOG_HW_TYPE__LAUNCHPAD();
#else // Neither platform is defined: this should never happen
    #error "Invalid platform"
#endif
    // TODO: Re-design startup
    // print_startup_type();
    // log_PBIST_fails();
    // obc_startup_logs();

    // Start the watchdog task once all other tasks are running
    obc_watchdog_start_task();

    while (1) {
        // Never schedule this again
        vTaskSuspend(NULL);
    }
}

/**
 * @brief entry point for the application. Performs any configuration
 * that must be done before the RTOS is started and then creates the
 * main task and starts the RTOS.
 *
 * @note This is called from `sys_main.c` generated by HALCoGEN on all
 * platforms.
 */
void obc_main(void) {
    // Turn off sleep in the idle task by default.
    idle_sleep_off();

    /**
     * RTI needs to be initialized here before @ref vTaskStartScheduler runs. This is
     * because the HALCoGen RTI initialization overwrites configuration values that the
     * RTOS needs to run.
     *
     * If, for example, the RTI is initialized in the main task, it will overwrite the RTOS
     * RTI configuration and prevent the RTOS from running. Initializing it before the RTOS
     * allows the RTOS to write its configuration during @ref vTaskStartScheduler and operate
     * normally.
     *
     * The RTI driver needs to be enabled in HALCOoGen by checking "Enable RTI driver" in the
     * "Driver Enabled" tab in order for rtiInit to be generated.
     */
    rtiInit();

    /**
     * The main task is where all of the top level tasks will be created from.
     *
     * This task is created with portPRIVILEGE_BIT set in order for the processor to
     * remain in Privileged mode. The Memory Protection Unit (MPU) will therefore be
     * disabled for this task, allowing it to do things such as configuring the
     * system's peripherals.
     *
     * https://www.freertos.org/FreeRTOS-MPU-memory-protection-unit.html
     */
    obc_rtos_create_task_privileged(OBC_TASK_ID_MAIN, &obc_main_task, NULL, OBC_WATCHDOG_ACTION_IGNORE);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    while (1) {
        // Keep running the scheduler forever.
    }
}
