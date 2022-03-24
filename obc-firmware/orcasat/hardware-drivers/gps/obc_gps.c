/**
 * @file obc_gps.h
 * @brief OEM7X GPS module driver
 * @author Andres Martinez
 * @date May 13, 2020
 *
 * @todo Remove GPS-UART loopback for module testing
 */

#include "obc_gps.h"

#include <string.h>

#include "obc_utils.h"
#include "obc_hardwaredefs.h"
#include "obc_uart.h"
#include "obc_misra.h"
#include "logger.h"
#include "rtos.h"
#include "assert.h"

#define GPS_ACQUISITION_POLL_INTERVAL_MS 1000
#define GPS_ACQUISITION_MAX_TIMEOUT_S    300

static TimerHandle_t xGpsAcqTimer;
static SemaphoreHandle_t xGpsMutex;

static void gps_acq_timer_callback(TimerHandle_t timer_handle);
static inline bool is_xyz_accurate(const gps_ecef_t* ec);
static inline bool is_time_accurate(const gps_time_t* ts);
static gps_req_err_t gps_request_and_receive(uint16_t timeout_s, const char* req_msg, gps_parse_err_t (*parsing_func)(char* ob, void* os), bool (*accuracy_check_func)(const void* os), void* gps_data);

/**
 * @brief Creates the FreeRTOS objects required for GPS module driver. This function must be done
 * before tasks call the GPS data request functions.
 */
void gps_create_infra(void) {
    static StaticSemaphore_t xGpsMutexBuffer;
    static StaticTimer_t xGpsAcqTimerBuffer;

    xGpsMutex    = xSemaphoreCreateMutexStatic(&xGpsMutexBuffer);
    xGpsAcqTimer = xTimerCreateStatic("GPS acquisition timer", pdMS_TO_TICKS(1000), pdFALSE, NULL, &gps_acq_timer_callback, &xGpsAcqTimerBuffer);
}

/**
 * @brief Reset the GPS module
 */
void gps_reset(void) {
    // power off
    // delay
    // power on
    // delay
}

/**
 * @brief Set the GPS load pin
 *
 * @param is_on[in]: Load pin on/off state
 */
void gps_set_power(bool is_on) {
    // if (is_on) vcc on
    // else vcc off
}

/**
 * @brief xGpsAcqTimer callback
 */
static void gps_acq_timer_callback(TimerHandle_t timer_handle) {
}

/**
 * @brief Check the parsed XYZ data for SOL_COMPUTED accuracy
 *
 * @param ec[in]: Parsed ECEF position & velocity
 */
static inline bool is_xyz_accurate(const gps_ecef_t* ec) {
    const bool pos_acc = !strcmp(ec->p_sol_status, "SOL_COMPUTED");
    const bool vel_acc = !strcmp(ec->v_sol_status, "SOL_COMPUTED");
    return pos_acc && vel_acc;
}

/**
 * @brief Check the parsed time data for FINESTEERING accuracy
 *
 * @param ts[in]: Parsed GPS time
 */
static inline bool is_time_accurate(const gps_time_t* ts) {
    return !strcmp(ts->t_sol_status, "FINESTEERING");
}

/**
 * @brief GPS driver request-receive-parse-check handler loop
 * On successful parse of a valid response message, will either return the result immediately (if timeout_s is zero),
 * or keep looping until an accurate result is acquired (or times out after timeout_s seconds)
 *
 * @param timeout_s[in]: Poll for accurate solution until timeout (>0) or just get the immediate response (0). Bound to
 * maximum value of @ref GPS_ACQUISITION_MAX_TIMEOUT_S (s)
 * @param req_msg[in]: Data request command to be sent to the GPS module
 * @param parsing_func[in]: Function that will parse the GPS module response message and store the
 * result in gps_data. If NULL then no parsing occurs and response string is copied to gps_data
 * @param accuracy_check_func[in]: Function that will check the gps_data result for an accurate
 * result
 * @param gps_data[out]: Processed GPS data. If parsing_func is NULL, then this should be an byte array of at least GPS_RX_BUFFER_SIZE,
 * otherwise a pointer to the applicable result struct
 *
 * @return Result status - GPS_SUCCESS if data successfully obtained, error code otherwise
 */
static gps_req_err_t gps_request_and_receive(uint16_t timeout_s, const char* req_msg, gps_parse_err_t (*parsing_func)(char* ob, void* os), bool (*accuracy_check_func)(const void* os),
                                             void* gps_data) {
    // Avoid simultaneous use of the GPS-UART and enforce strict 1-to-1 send->receive pattern.
    if (xSemaphoreTake(xGpsMutex, pdMS_TO_TICKS(GPS_ACQUISITION_MAX_TIMEOUT_S * 1000)) == pdFALSE) {
        return GPS_MUTEX_TIMEOUT;
    }

    // Restrict acquisition timeout to maximum
    if (timeout_s > GPS_ACQUISITION_MAX_TIMEOUT_S) {
        timeout_s = GPS_ACQUISITION_MAX_TIMEOUT_S;
    }
    // If waiting for an accurate result, then start the acquisition timeout
    if (timeout_s) {
        if (xTimerChangePeriod(xGpsAcqTimer, pdMS_TO_TICKS(timeout_s * 1000), 0) == pdFAIL) {
            log_str(ERROR, GPS_LOG, true, "GPS ACQ timer set period fail");
            return GPS_ACQ_TIMEOUT;
        }
        if (xTimerStart(xGpsAcqTimer, 0) == pdFAIL) {
            log_str(ERROR, GPS_LOG, true, "GPS ACQ timer init fail");
            return GPS_ACQ_TIMEOUT;
        }
        // Verify that the timer has actually been started by the scheduler before proceeding..
        while (!xTimerIsTimerActive(xGpsAcqTimer)) {
            vTaskDelay(1);
        }
    }

    static char resp_buff[GPS_RX_BUFFER_SIZE];
    while (1) {
        const uart_err_t uart_err = gps_serial_send_and_receive(req_msg, resp_buff);
        if (uart_err != UART_OK) {
            log_str(ERROR, GPS_LOG, false, "GPS UART err %d", uart_err);
            xSemaphoreGive(xGpsMutex);
            return GPS_UART_ERR;
        }

        // Apply specified data parser to the response string
        if (parsing_func) {
            const gps_parse_err_t parse_err = parsing_func(resp_buff, gps_data);
            if (parse_err != GPS_PARSE_SUCCESS) {
                log_str(ERROR, GPS_LOG, false, "GPS parse err %d", parse_err);
                xSemaphoreGive(xGpsMutex);
                return GPS_MSG_PARSE_ERR;
            }
        }
        // If no parser specified, then return the response string directly
        else {
            strncpy(gps_data, resp_buff, GPS_RX_BUFFER_SIZE);
            xSemaphoreGive(xGpsMutex);
            return GPS_SUCCESS;
        }

        // If the result is accurate or an accurate result is not requested then return success
        if ((accuracy_check_func(gps_data)) || (timeout_s == 0)) {
            xSemaphoreGive(xGpsMutex);
            return GPS_SUCCESS;
        }
        // Else if the timer has expired then return timeout
        else if (!xTimerIsTimerActive(xGpsAcqTimer)) {
            log_str(ERROR, GPS_LOG, false, "GPS acquisition timeout");
            xSemaphoreGive(xGpsMutex);
            return GPS_ACQ_TIMEOUT;
        }
        // Else delay for another GPS request-and-receive loop
        else {
            log_str(INFO, GPS_LOG, false, "GPS module acquiring...");
            vTaskDelay(pdMS_TO_TICKS(GPS_ACQUISITION_POLL_INTERVAL_MS));
        }
    }
}

/**
 * @brief Request and receive the GPS module position and velocity data
 *
 * @param timeout_s[in]: Poll for SOL_COMPUTED acquisition until timeout (>0) or just get the current log (0)
 * @param ec[out]: Parsed ECEF position & velocity
 *
 * @return Result status - GPS_SUCCESS if data successfully obtained, error code otherwise
 */
gps_req_err_t gps_req_xyz(uint16_t timeout_s, gps_ecef_t* ec) {
    return gps_request_and_receive(timeout_s, "LOG BESTXYZA ONNEXT\r\n",
                                   // Suppress MISRA warning about function pointer conversion (to support conversion of the
                                   // GPS data parameters to void*)
                                   OBC_MISRA_CHECK_OFF(gps_parse_err_t(*)(char* ob, void* os)) & parse_gps_bestxyz_ascii, (bool (*)(const void* os)) & is_xyz_accurate, OBC_MISRA_CHECK_ON ec);
}

/**
 * @brief Request and receive the GPS module time data
 *
 * @param timeout_s[in]: Poll for FINESTEERING acquisition until timeout (>0) or just get the current log (0)
 * @param ts[out]: Parsed GPS time
 *
 * @return Result status - GPS_SUCCESS if data successfully obtained, error code otherwise
 */
gps_req_err_t gps_req_time(uint16_t timeout_s, gps_time_t* ts) {
    return gps_request_and_receive(timeout_s, "LOG TIMESYNCA ONNEXT\r\n",
                                   // Suppress MISRA warning about function pointer conversion (to support conversion of the
                                   // GPS data parameters to void*)
                                   OBC_MISRA_CHECK_OFF(gps_parse_err_t(*)(char* ob, void* os)) & parse_gps_timesync_ascii, (bool (*)(const void* os)) & is_time_accurate, OBC_MISRA_CHECK_ON ts);
}

/**
 * @brief Send a string to the OEM7X GPS module and receive the response string.
 * Intended to be used for sending arbitrary user-specified commands to the module (i.e. status request, reset)
 *
 * @param req_msg[in]: Command string to be sent to the OEM7X. Must be terminated with \r\n
 * @param gps_response_msg[out]: GPS-UART response buffer. Size must be at least @ref GPS_RX_BUFFER_SIZE
 *
 * @return Result status - GPS_SUCCESS if response successfully obtained, error code otherwise
 */
gps_req_err_t gps_cmd(const char* req_msg, char* resp_msg) {
    return gps_request_and_receive(0, req_msg, NULL, NULL, resp_msg);
}
