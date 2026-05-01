/*================================ FILE INFO =================================*/
/* Filename           : test_device_self_tests.cpp                            */
/*                                                                            */
/* Test implementation for device_self_tests.c                                */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdint.h>
#include "device_self_tests.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <array>
#include <cstdio>
#include <fstream>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

/* ---------------------------------------------------------------------------*/
/* processor mocks */
void delay_ms(uint32_t delay_time)
{
    mock().actualCall("delay_ms");
}

void delay_us(uint32_t delay_time)
{
    mock().actualCall("delay_us");
}

void start_timer(void)
{
    mock().actualCall("start_timer");
}

void reset_timer(void)
{
    mock().actualCall("reset_timer");
}

uint32_t get_current_time_ms(void)
{
    return mock().actualCall("get_current_time_ms")
        .returnUnsignedIntValue();
}

/* ---------------------------------------------------------------------------*/
/* power enabler mocks */
void enable_power(void)
{
    mock().actualCall("enable_power");
}

void disable_power(void)
{
    mock().actualCall("disable_power");
}

/* ---------------------------------------------------------------------------*/
/* battery comparator mocks */
bool is_battery_low(void)
{
    return mock().actualCall("is_battery_low")
        .returnBoolValue();
}

/* ---------------------------------------------------------------------------*/
/* led mocks */
void set_led_d1_enabled(bool enabled)
{
    mock().actualCall("set_led_d1_enabled")
        .withBoolParameter("enabled", enabled);
}

void set_led_d2_enabled(bool enabled)
{
    mock().actualCall("set_led_d2_enabled")
        .withBoolParameter("enabled", enabled);
}

void set_led_d3_enabled(bool enabled)
{
    mock().actualCall("set_led_d3_enabled")
        .withBoolParameter("enabled", enabled);
}

void set_led_d4_enabled(bool enabled)
{
    mock().actualCall("set_led_d4_enabled")
        .withBoolParameter("enabled", enabled);
}

/* ---------------------------------------------------------------------------*/
/* pushbutton mocks */
uint32_t get_pushbutton_count(void)
{
    return mock().actualCall("get_pushbutton_count")
        .returnUnsignedIntValue();
}

void clear_pushbutton_count(void)
{
    mock().actualCall("clear_pushbutton_count");
}

/* ---------------------------------------------------------------------------*/
/* infrared sensor mocks */
bool read_ir_1_sensor_called{false};
bool read_ir_2_sensor_called{false};
bool read_ir_3_sensor_called{false};
bool read_ir_4_sensor_called{false};

uint32_t read_ir_1_sensor(void)
{
    return read_ir_1_sensor_called = true;
}

uint32_t read_ir_2_sensor(void)
{
    return read_ir_2_sensor_called = true;
}

uint32_t read_ir_3_sensor(void)
{
    return read_ir_3_sensor_called = true;
}

uint32_t read_ir_4_sensor(void)
{
    return read_ir_4_sensor_called = true;
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
FILE *standard_output{nullptr};
constexpr const char *TEST_FILE{"test_output.txt"};

void redirect_stdout_to_file(void)
{
    standard_output = stdout;
    CHECK(freopen(TEST_FILE, "w+", stdout) != nullptr);
}

void check_printf_output(const char *expected_string)
{
    constexpr std::size_t MAX_BUFFER_SIZE{1024};
    std::array<char, MAX_BUFFER_SIZE> buffer{};

    FILE* file{fopen(TEST_FILE, "r")};
    CHECK(file != nullptr);
    
    const size_t bytes_read{fread(buffer.data(), 1, buffer.size() - 1, file)};
    CHECK(bytes_read > 0);
    fclose(file);

    STRCMP_EQUAL(expected_string, buffer.data());
}

void restore_stdout(void)
{
    CHECK(stdout != nullptr);
    fclose(stdout);
    CHECK(freopen("CON", "w", standard_output) != nullptr);
}

void reset_all_mock_flags(void)
{
    read_ir_1_sensor_called = false;
    read_ir_2_sensor_called = false;
    read_ir_3_sensor_called = false;
    read_ir_4_sensor_called = false;
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(DeviceSelfTestsTests)
{
    void setup() override
    {
        reset_all_mock_flags();
        mock().clear();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
        reset_all_mock_flags();
    }
};

/*============================================================================*/
/*                                    Tests                                   */
/*============================================================================*/
TEST(DeviceSelfTestsTests, InitDeviceSelfTests)
{
    init_device_self_tests();
}

TEST(DeviceSelfTestsTests, DeinitDeviceSelfTests)
{
    deinit_device_self_tests();
}

TEST(DeviceSelfTestsTests, ProcessorTestsPrintsHelloWorldAndDelays)
{
    mock().expectOneCall("start_timer");
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(0);
    mock().expectOneCall("delay_ms");
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(1000);
    mock().expectOneCall("reset_timer");
    mock().expectOneCall("delay_us");
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(1000);

    redirect_stdout_to_file();
    processor_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("time: 0ms\r\n"
                        "Hello World\r\n"
                        "time: 1000ms... resetting timer\r\n"
                        "Konnichiwa Sekai\r\n"
                        "time: 1000ms\r\n");
}

TEST(DeviceSelfTestsTests, BatteryComparatorTestPrintsOnGoodBattery)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("disable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms");

    redirect_stdout_to_file();
    battery_comparator_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("power enabled\r\nBattery is not low\r\n"
                        "power disabled\r\nBattery is not low\r\n");
}

TEST(DeviceSelfTestsTests, BatteryComparatorTestPrintsOnBadBattery)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(true);
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("disable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(true);
    mock().expectOneCall("delay_ms");

    redirect_stdout_to_file();
    battery_comparator_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("power enabled\r\nBattery is low\r\n"
                        "power disabled\r\nBattery is low\r\n");
}

TEST(DeviceSelfTestsTests, PowerEnablerTestPrintsAndDelays)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("disable_power");
    mock().expectOneCall("delay_ms");

    redirect_stdout_to_file();
    power_enabler_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("power enabled\r\npower disabled\r\n");
}

TEST(DeviceSelfTestsTests, LedTestTogglesLeds)
{
    mock().expectOneCall("set_led_d1_enabled")
        .withBoolParameter("enabled", true);
    mock().expectOneCall("set_led_d2_enabled")
        .withBoolParameter("enabled", true);
    mock().expectOneCall("set_led_d3_enabled")
        .withBoolParameter("enabled", true);
    mock().expectOneCall("set_led_d4_enabled")
        .withBoolParameter("enabled", true);
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("set_led_d1_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d2_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d3_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d4_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("delay_ms");

    led_test();
}

TEST(DeviceSelfTestsTests, PushbuttonTestPrintsCount)
{
    for (int i{0}; i < 5; i++) {
        mock().expectOneCall("get_pushbutton_count")
            .andReturnValue(0);
        mock().expectOneCall("delay_ms");
    }
    mock().expectOneCall("clear_pushbutton_count");
    mock().expectOneCall("get_pushbutton_count")
        .andReturnValue(0);

    redirect_stdout_to_file();
    pushbutton_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("pushbutton count: 0\r\n"
                        "pushbutton count: 0\r\n"
                        "pushbutton count: 0\r\n"
                        "pushbutton count: 0\r\n"
                        "pushbutton count: 0\r\n"
                        "cleared pushbutton count: 0\r\n");
}

TEST(DeviceSelfTestsTests, IrSensorsDistanceTestCallsFunctions)
{
    constexpr int SENSOR_COUNT{4};
    constexpr int TOTAL_TEST_DISTANCES{14};
    constexpr uint32_t TRIALS_COUNT{2u};
    constexpr uint32_t TIME_PER_TRIAL_SEC{3u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");
    for (int s{0}; s < SENSOR_COUNT; s++) {
        for (int d{0}; d < TOTAL_TEST_DISTANCES; d++) {
            for (int t{0}; t < TRIALS_COUNT; t++) {
                mock().expectOneCall("delay_ms");
                mock().expectOneCall("reset_timer");
                mock().expectOneCall("get_current_time_ms")
                    .andReturnValue(0);
                for (int timer_iteration{0}; timer_iteration < 2; timer_iteration++) {
                    if (timer_iteration == 0) {
                        mock().expectOneCall("get_current_time_ms")
                            .andReturnValue(0);
                    } else {
                        mock().expectOneCall("get_current_time_ms")
                            .andReturnValue(TIME_PER_TRIAL_SEC * 1000);
                    }
                }
            }
        }
    }
    mock().expectOneCall("disable_power");

    infrared_sensors_distance_test(TRIALS_COUNT);
    CHECK(read_ir_1_sensor_called);
    CHECK(read_ir_2_sensor_called);
    CHECK(read_ir_3_sensor_called);
    CHECK(read_ir_4_sensor_called);
}

TEST(DeviceSelfTestsTests, IrSensorsFreeReadingTestCallsFunctions)
{
    constexpr int SENSOR_COUNT{4};
    constexpr uint32_t TIME_PER_SENSOR_SEC{30u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");
    for (int s{0}; s < SENSOR_COUNT; s++) {
        mock().expectOneCall("delay_ms");
        mock().expectOneCall("reset_timer");
        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(0);
        for (int timer_iteration{0}; timer_iteration < 2; timer_iteration++) {
            if (timer_iteration == 0) {
                mock().expectOneCall("get_current_time_ms")
                    .andReturnValue(0);
            } else {
                mock().expectOneCall("get_current_time_ms")
                    .andReturnValue(TIME_PER_SENSOR_SEC * 1000);
            }
        }
    }
    mock().expectOneCall("disable_power");

    infrared_sensors_free_reading_test();
    CHECK(read_ir_1_sensor_called);
    CHECK(read_ir_2_sensor_called);
    CHECK(read_ir_3_sensor_called);
    CHECK(read_ir_4_sensor_called);
}

TEST(DeviceSelfTestsTests, IrSensorsReadSpeedTestCallsFunctions)
{
    constexpr int SENSOR_COUNT{4};
    constexpr uint32_t TIME_PER_SENSOR_SEC{5u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");
    for (int s{0}; s < SENSOR_COUNT; s++) {
        mock().expectOneCall("reset_timer");
        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(0);
        for (int timer_iteration{0}; timer_iteration < 2; timer_iteration++) {
            if (timer_iteration == 0) {
                mock().expectOneCall("get_current_time_ms")
                    .andReturnValue(0);
            } else {
                mock().expectOneCall("get_current_time_ms")
                    .andReturnValue(TIME_PER_SENSOR_SEC * 1000);
            }
        }
    }
    mock().expectOneCall("disable_power");

    infrared_sensors_read_speed_test();
    CHECK(read_ir_1_sensor_called);
    CHECK(read_ir_2_sensor_called);
    CHECK(read_ir_3_sensor_called);
    CHECK(read_ir_4_sensor_called);
}
