/**
 * @file obc_main.c
 * @brief Entry point for the application and the main FreeRTOS task
 * that creates all other tasks.
 * @author Richard Arthurs
 * @date April 18, 2020
 */
#include "obc_main.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "sys_core.h"
#include "rti.h"
#include "het.h"
#include "low_power.h"
#include "obc_rtos.h"
#include "obc_temperature.h"
#include "obc_task_info.h"
#include "obc_featuredefs.h"
#include "obc_rtc.h"
#include "obc_uart.h"
#include "obc_gps.h"
#include "obc_mibspi.h"
#include "tms_mibspi.h"
#include "obc_startup.h"
#include "obc_flash.h"
#include "obc_watchdog.h"
#include "obc_gpio.h"
#include "obc_cmd.h"
#include "blinky.h"
#include "unit_tests.h"
#include "logger.h"
#include "i2c_freertos.h"
#include "scheduler.h"
#include "filesystem.h"
#include "orca_telem_logger.h"
#include "hang.h"
#include "obc_settings.h"
#include "obc_adc.h"
#include "obc_mram.h"
#include "obc_comms.h"
#include "tms_can.h"
#include "tms_spi.h"

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
    wd_create_infra();
    uart_create_infra();
    i2c_create_infra();
    gpio_create_infra();
    rtc_create_infra();
    scheduler_create_infra();
    obc_rtos_create_infra();
    telem_create_infra();
    gps_create_infra();

    // Start the backup epoch so we have a timestamp before initializing the hardware RTCs.
    // If errors occur in subsequent steps, they will be able to properly log the error because
    // the backup epoch is providing a time reference.
    rtc_init_backup();

    // Initialize low-level hardware for all features, but do not enable interrupts for most
    // features yet. Interrupts cannot be safely enabled until the tasks that deal with handling the
    // interrupts have been created. Hardware features where interrupts are enabled during
    // init are tagged with "_irq" in the init function name.
    uart_init_hw();
    i2c_init_irq_hw(); // OBC is master on I2C, so it is okay that we enable interrupts in this
                       // function despite not having started the I2C task yet.
    gpio_init_hw();
    mibspi_init_hw(); // OBC is master on MIBSPI, so no interrupts will occur without the OBC
                          // having enough infrastructure created to process them.
    mibspi_create_infra();
    comms_mibspi_create_infra();
    tms_spi_create_infra();
    comms_mibspi_init_irq();
    hetInit();
    adc_init();
    tms_can_init();
    tms_spi_init();

    // Every other task requires that the watchdog pet task (WDPT) be working, because all other
    // tasks use the watchdog check-in functionality.
    wd_start_task();

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
    uart_start_task();
    gpio_start_task();
    i2c_start_task();

    // Start the GPIO blinky task. This is a useful indicator because if anything is really wrong
    // with startup, it might stop blinking.
    blinky_start_task();

    // Print now that enough features have been initialized to make it happen.
    log_str_no_time(INFO, PRINT_GENERAL, false, "Startup");

    // Initialize the complex external hardware.
    // The RTC requires MIBSPI to be working before it can be initialized.
    // The GPIO expander requires I2C to be working before it can be initialized.
    rtc_init();
    gpio_expander_init();

    // Start the hang task.
    hang_start_task();

    // Enable interrupts. This is safe to do now that the interrupt processing tasks have been
    // started.
    gpio_init_irq();

    // Start the scheduler tasks so that commands can be processed. It's best to do this after the
    // RTC is started so there is a good time reference.
    scheduler_start();

    // Enable interrupts for the UART. This should only be done after the scheduler is started,
    // because the scheduler processes commands received from the UART.
    uart_init_irq();

    // Filesystem initialization requires flash, and takes a couple of seconds because we erase it
    // at startup.
    flash_init();
    fs_init();

    // Create the telemetry logging task. This requires that the filesystem is ready to go.
    telem_start_task();

    // Hardware is ready to go now. Print out some information about startup.
    log_str(INFO, PRINT_GENERAL, false, "ALEASAT Started");
    log_str(INFO, HW_TYPE, false, BOARD_TYPE_MSG);
    print_startup_type();
    log_PBIST_fails();
    obc_startup_logs();

    // Create the internally scheduled actions
    obc_cmd_parse_and_invoke("0 count 20-01-01 0:0:10 5", INTERNAL);
    // Create any other tasks.
    // task_create(&vLogSysTestTask, "Log", 400, NULL, BLINKY_TASK_DEFAULT_PRIORITY,
    // &xLogSysTaskHandle );

    vTaskDelay(pdMS_TO_TICKS(10)); // NECESSARY: expander blinker doesn't work without brief delay (waiting for hw init?)
    expander_blinky_start_task();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    static StaticTask_t main_task_buf                        = {NULL};
    static StackType_t main_task_stack[MAIN_TASK_STACK_SIZE] = {NULL};
    xTaskCreateStatic(&obc_main_task, "main", MAIN_TASK_STACK_SIZE, NULL, portPRIVILEGE_BIT | MAIN_TASK_PRIORITY, main_task_stack, &main_task_buf);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    while (1) {
        // Keep running the scheduler forever.
    }
}
