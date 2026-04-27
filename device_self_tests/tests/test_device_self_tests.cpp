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
#include "processor.h"
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

void delay_ms(uint32_t delay_time)
{
    mock().actualCall("delay_ms");
}

void delay_us(uint32_t delay_time)
{
    mock().actualCall("delay_us");
}

void enable_power(void)
{
    mock().actualCall("enable_power");
}

void disable_power(void)
{
    mock().actualCall("disable_power");
}

bool is_battery_low(void)
{
    return mock().actualCall("is_battery_low")
        .returnBoolValue();
}

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

uint32_t get_pushbutton_count(void)
{
    return mock().actualCall("get_pushbutton_count")
        .returnUnsignedIntValue();
}

void clear_pushbutton_count(void)
{
    mock().actualCall("clear_pushbutton_count");
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

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(DeviceSelfTestsTests)
{
    void setup() override
    {
        mock().clear();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
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
    mock().expectOneCall("delay_ms");
    mock().expectOneCall("delay_us");

    redirect_stdout_to_file();
    processor_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("Hello World\r\nKonnichiwa Sekai\r\n");
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
