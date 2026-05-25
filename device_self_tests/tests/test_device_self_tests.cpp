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

/* ---------------------------------------------------------------------------*/
/* wheel motor fakes */
int32_t fake_ticks{0};

int32_t fake_get_ticks(void)
{
    return fake_ticks++;
}

int32_t fake_get_ticks_no_movement(void)
{
    return 0;
}

void fake_set_speed(uint8_t speed)
{
    /* nothing */
}

void fake_clear_ticks(void)
{
    fake_ticks = 0;
}

/* ---------------------------------------------------------------------------*/
/* vacuum mocks */
void set_vacuum_speed(uint8_t speed)
{
    mock().actualCall("set_vacuum_speed");
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
void reset_all_mock_variables(void)
{
    read_ir_1_sensor_called = false;
    read_ir_2_sensor_called = false;
    read_ir_3_sensor_called = false;
    read_ir_4_sensor_called = false;
    fake_ticks = 0;
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(DeviceSelfTestsTests)
{
    void setup() override
    {
        reset_all_mock_variables();
        mock().clear();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
        reset_all_mock_variables();
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

TEST(DeviceSelfTestsTests, ProcessorTestCallsFunctions)
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

    processor_test();
}

TEST(DeviceSelfTestsTests, BatteryComparatorTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("disable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms");

    battery_comparator_test();
}

TEST(DeviceSelfTestsTests, PowerEnablerTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("delay_ms");

    mock().expectOneCall("disable_power");
    mock().expectOneCall("delay_ms");

    power_enabler_test();
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

    pushbutton_test();
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
            for (uint32_t t{0}; t < TRIALS_COUNT; t++) {
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

TEST(DeviceSelfTestsTests, MoveUntilEncoderCountReachesTarget)
{
    constexpr int32_t ENCODER_COUNT{50};

    mock().expectNCalls((ENCODER_COUNT / 2) + 2, "get_current_time_ms")
        .andReturnValue(0);
    mock().expectNCalls(1, "delay_ms");

    move_until_encoder_count(ENCODER_COUNT, 100, fake_set_speed, fake_get_ticks, fake_clear_ticks);
}

TEST(DeviceSelfTestsTests, MoveUntilEncoderCountBreaksOnTimeout)
{
    constexpr int32_t ENCODER_COUNT{50};

    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(0);
    mock().expectNCalls(2, "get_current_time_ms")
        .andReturnValue(3000);
    mock().expectNCalls(1, "delay_ms");

    move_until_encoder_count(ENCODER_COUNT, 100, fake_set_speed, fake_get_ticks_no_movement, fake_clear_ticks);
}

TEST(DeviceSelfTestsTests, VacuumTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    for (uint8_t motor_speed = 0u; motor_speed < 255; motor_speed++) {
        mock().expectOneCall("set_vacuum_speed");
        mock().expectOneCall("delay_ms");
    }
    mock().expectOneCall("disable_power");

    vacuum_test();
}
