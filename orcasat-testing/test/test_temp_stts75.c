#include "unity.h"
#include "temp_stts75.h"
#include "obc_error.h"

#include "FreeRTOS.h"
#include "mock_rtos_portable.h"
#include "mock_rtos_portmacro.h"
#include "mock_mpu_prototypes.h"

#include "mock_obc_utils.h"
#include "mock_logger.h"
#include "mock_i2c.h"
#include "mock_obc_i2c.h"
#include "mock_i2c_freertos.h"

QueueHandle_t temp_sensor_rx_q = (void*)1; // We need something for temp_sensor_rx_q to be found
uint8_t success                = I2C_SUCCESS;

// tests for bytes to celsius conversion
void test_temp_bytes_to_celsius_0();
void test_temp_bytes_to_celsius_0_5();
void test_temp_bytes_to_celsius_10();
void test_temp_bytes_to_celsius_25();
void test_temp_bytes_to_celsius_41();
void test_temp_bytes_to_celsius_125();
void test_temp_bytes_to_celsius_neg_0_5();
void test_temp_bytes_to_celsius_neg_10();
void test_temp_bytes_to_celsius_neg_25();
void test_temp_bytes_to_celsius_neg_55();

// tests for celsius to byte conversion (depend on first set of tests passing)
void test_temp_celsius_to_bytes_125();
void test_temp_celsius_to_bytes_25();
void test_temp_celsius_to_bytes_10();
void test_temp_celsius_to_bytes_1();
void test_temp_celsius_to_bytes_0();
void test_temp_celsius_to_bytes_neg_10();
void test_temp_celsius_to_bytes_neg_25();
void test_temp_celsius_to_bytes_neg_55();

// test helpers
static void mock_receive();
static void give_x_degrees(i2c_command_t command, uint8_t* data);

static int8_t give_41_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_125_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_25_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_10_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_0_5_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_0_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_neg_55_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_neg_25_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_neg_10_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t give_neg_0_5_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls);

static int8_t byte_response_125(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_25(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_10(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_1(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_0(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_neg_10(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_neg_25(i2c_command_t command, UBaseType_t priority, int NumCalls);
static int8_t byte_response_neg_55(i2c_command_t command, UBaseType_t priority, int NumCalls);

void setUp(void) {
}

void tearDown(void) {
}

void test_temp_stlm75(void) {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_41_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);

    TEST_ASSERT_INT16_WITHIN(1, 41, actualTemp);
}

void test_temp_bytes_to_celsius_0() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_0_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 0, actualTemp);
}

void test_temp_bytes_to_celsius_0_5() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_0_5_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 0.5, actualTemp);
}

void test_temp_bytes_to_celsius_10() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_10_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 10, actualTemp);
}

void test_temp_bytes_to_celsius_25() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_25_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 25, actualTemp);
}

void test_temp_bytes_to_celsius_41() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_41_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 41, actualTemp);
}

void test_temp_bytes_to_celsius_125() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_125_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, 125, actualTemp);
}

void test_temp_bytes_to_celsius_neg_0_5() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_neg_0_5_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, -0.5, actualTemp);
}

void test_temp_bytes_to_celsius_neg_10() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_neg_10_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, -10, actualTemp);
}

void test_temp_bytes_to_celsius_neg_25() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_neg_25_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, -25, actualTemp);
}

void test_temp_bytes_to_celsius_neg_55() {
    int16_t actualTemp = -1;

    send_command_to_i2c_worker_StubWithCallback(&give_neg_55_degrees);
    mock_receive();
    stts75_temp_get(&actualTemp);
    TEST_ASSERT_INT16_WITHIN(1, -55, actualTemp);
}

void test_temp_celsius_to_bytes_125() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_125);
    mock_receive();
    stts75_hysteresis_set(125);
}

void test_temp_celsius_to_bytes_25() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_25);
    mock_receive();
    stts75_hysteresis_set(25);
}

void test_temp_celsius_to_bytes_10() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_10);
    mock_receive();
    stts75_hysteresis_set(10);
}

void test_temp_celsius_to_bytes_1() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_1);
    mock_receive();
    stts75_hysteresis_set(1);
}

void test_temp_celsius_to_bytes_0() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_0);
    mock_receive();
    stts75_hysteresis_set(0);
}

void test_temp_celsius_to_bytes_neg_10() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_neg_10);
    mock_receive();
    stts75_hysteresis_set(-10);
}

void test_temp_celsius_to_bytes_neg_25() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_neg_25);
    mock_receive();
    stts75_hysteresis_set(-25);
}

void test_temp_celsius_to_bytes_neg_55() {
    send_command_to_i2c_worker_StubWithCallback(&byte_response_neg_55);
    mock_receive();
    stts75_hysteresis_set(-55);
}

//____________ HERE BE HELPERS _________________

// Bytes to Celsius helpers

static int8_t give_41_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_41[2] = {0x29, 0x00};
    give_x_degrees(command, temp_41);

    return I2C_SUCCESS;
}

static int8_t give_125_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_125[2] = {0x7D, 0x00};
    give_x_degrees(command, temp_125);

    return I2C_SUCCESS;
}

static int8_t give_25_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_25[2] = {0x19, 0x00};
    give_x_degrees(command, temp_25);

    return I2C_SUCCESS;
}

static int8_t give_10_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_10[2] = {0x0A, 0x00};
    give_x_degrees(command, temp_10);

    return I2C_SUCCESS;
}

static int8_t give_0_5_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_0_5[2] = {0x00, 0x80};
    give_x_degrees(command, temp_0_5);

    return I2C_SUCCESS;
}

static int8_t give_0_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_0[2] = {0x00, 0x00};
    give_x_degrees(command, temp_0);

    return I2C_SUCCESS;
}

static int8_t give_neg_55_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_neg_55[2] = {0xC9, 0x00};
    give_x_degrees(command, temp_neg_55);

    return I2C_SUCCESS;
}

static int8_t give_neg_25_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_neg_25[2] = {0xE7, 0x00};
    give_x_degrees(command, temp_neg_25);

    return I2C_SUCCESS;
}

static int8_t give_neg_10_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_neg_10[2] = {0xF6, 0x00};
    give_x_degrees(command, temp_neg_10);

    return I2C_SUCCESS;
}

static int8_t give_neg_0_5_degrees(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    uint8_t temp_neg_0_5[2] = {0xFF, 0x80};
    give_x_degrees(command, temp_neg_0_5);

    return I2C_SUCCESS;
}

static void give_x_degrees(i2c_command_t command, uint8_t* data) {
    command.data[0] = data[0];
    command.data[1] = data[1];
}

// Celsius to bytes helper
static int8_t byte_response_125(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0x7D00, byte_val);
}

static int8_t byte_response_25(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0x1900, byte_val);
}

static int8_t byte_response_10(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0x0A00, byte_val);
}

static int8_t byte_response_1(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0x0100, byte_val);
}

static int8_t byte_response_0(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0x0000, byte_val);
}

static int8_t byte_response_neg_10(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0xF600, byte_val);
}

static int8_t byte_response_neg_25(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0xE700, byte_val);
}

static int8_t byte_response_neg_55(i2c_command_t command, UBaseType_t priority, int NumCalls) {
    int16_t byte_val = (command.data[0] << 8) + command.data[1];
    TEST_ASSERT_INT16_WITHIN(1, 0xC900, byte_val);
}

static void mock_receive() {
    MPU_xQueueGenericReceive_ExpectAndReturn(0, 0, 0, 0, pdTRUE);
    MPU_xQueueGenericReceive_IgnoreArg_xQueue();
    MPU_xQueueGenericReceive_IgnoreArg_pvBuffer();
    MPU_xQueueGenericReceive_IgnoreArg_xTicksToWait();
    MPU_xQueueGenericReceive_IgnoreArg_xJustPeek();
    MPU_xQueueGenericReceive_ReturnThruPtr_pvBuffer(&success);
}
