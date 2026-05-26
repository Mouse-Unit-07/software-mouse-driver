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
    mock().actualCall("delay_ms")
        .withUnsignedIntParameter("delay_time", delay_time);
}

void delay_us(uint32_t delay_time)
{
    mock().actualCall("delay_us")
        .withUnsignedIntParameter("delay_time", delay_time);
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
/* infrared sensor mocks and fakes */
uint32_t read_ir_1_sensor_call_count{0};
uint32_t read_ir_2_sensor_call_count{0};
uint32_t read_ir_3_sensor_call_count{0};
uint32_t read_ir_4_sensor_call_count{0};
uint32_t fake_ir_1_reading_value{100u};
uint32_t fake_ir_2_reading_value{100u};
uint32_t fake_ir_3_reading_value{100u};
uint32_t fake_ir_4_reading_value{100u};

uint32_t read_ir_1_sensor(void)
{
    read_ir_1_sensor_call_count++;
    return fake_ir_1_reading_value;
}

uint32_t read_ir_2_sensor(void)
{
    read_ir_2_sensor_call_count++;
    return fake_ir_2_reading_value;
}

uint32_t read_ir_3_sensor(void)
{
    read_ir_3_sensor_call_count++;
    return fake_ir_3_reading_value;
}

uint32_t read_ir_4_sensor(void)
{
    read_ir_4_sensor_call_count++;
    return fake_ir_4_reading_value;
}

/* ---------------------------------------------------------------------------*/
/* wheel motor mocks and fakes */
void set_wheel_motor_1_direction_forward(void)
{
    mock().actualCall("set_wheel_motor_1_direction_forward");
}

void set_wheel_motor_1_direction_backward(void)
{
    mock().actualCall("set_wheel_motor_1_direction_backward");
}

void set_wheel_motor_2_direction_forward(void)
{
    mock().actualCall("set_wheel_motor_2_direction_forward");
}

void set_wheel_motor_2_direction_backward(void)
{
    mock().actualCall("set_wheel_motor_2_direction_backward");
}

void set_wheel_motor_1_speed(uint8_t speed)
{
    mock().actualCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", speed);
}

void set_wheel_motor_2_speed(uint8_t speed)
{
    mock().actualCall("set_wheel_motor_2_speed")
        .withUnsignedIntParameter("speed", speed);
}

/* ---------------------------------------------------------------------------*/
/* encoder fakes */
int32_t fake_encoder_1_ticks{0};
int32_t fake_encoder_2_ticks{0};

int32_t get_encoder_1_ticks(void)
{
    return fake_encoder_1_ticks++;
}

int32_t get_encoder_2_ticks(void)
{
    return fake_encoder_2_ticks++;
}

int32_t fake_get_ticks_no_movement(void)
{
    return 0;
}

void clear_1_encoder_ticks(void)
{
    fake_encoder_1_ticks = 0;
}

void clear_2_encoder_ticks(void)
{
    fake_encoder_2_ticks = 0;
}

void fake_clear_ticks(void)
{
    /* nothing */
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
    read_ir_1_sensor_call_count = 0;
    read_ir_2_sensor_call_count = 0;
    read_ir_3_sensor_call_count = 0;
    read_ir_4_sensor_call_count = 0;
    fake_ir_1_reading_value = 100u;
    fake_ir_2_reading_value = 100u;
    fake_ir_3_reading_value = 100u;
    fake_ir_4_reading_value = 100u;
    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;
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
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 1000u);
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(1000);
    mock().expectOneCall("reset_timer");
    mock().expectOneCall("delay_us")
        .withUnsignedIntParameter("delay_time", 1000000u);
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(1000);

    processor_test();
}

TEST(DeviceSelfTestsTests, BatteryComparatorTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 2000);

    mock().expectOneCall("disable_power");
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 2000);

    battery_comparator_test();
}

TEST(DeviceSelfTestsTests, PowerEnablerTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 10000);

    mock().expectOneCall("disable_power");
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 10000);

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
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 1000);

    mock().expectOneCall("set_led_d1_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d2_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d3_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("set_led_d4_enabled")
        .withBoolParameter("enabled", false);
    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", 1000);

    led_test();
}

TEST(DeviceSelfTestsTests, PushbuttonTestCallsFunctions)
{
    for (int i{0}; i < 5; i++) {
        mock().expectOneCall("get_pushbutton_count")
            .andReturnValue(0);
        mock().expectOneCall("delay_ms")
            .withUnsignedIntParameter("delay_time", 1000);
    }
    mock().expectOneCall("clear_pushbutton_count");
    mock().expectOneCall("get_pushbutton_count")
        .andReturnValue(0);

    pushbutton_test();
}

TEST(DeviceSelfTestsTests, MeasureAverageReadingReturnsAverage)
{
    mock().expectOneCall("reset_timer");

    mock().expectNCalls(3, "get_current_time_ms")
        .andReturnValue(0);

    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(10);

    uint32_t avg = measure_average_reading(10, read_ir_1_sensor);

    CHECK(avg == fake_ir_1_reading_value);
}

TEST(DeviceSelfTestsTests, IrSensorsDistanceTestCallsFunctions)
{
    ir_distance_test_config cfg = {0};
    cfg.start_distance_cm = 1u;
    cfg.end_distance_cm = 2u;
    cfg.trials_per_distance = 1u;
    cfg.time_per_trial_ms = 10u;
    cfg.setup_delay_ms = 500u;

    constexpr uint32_t SENSOR_COUNT{4u};
    constexpr uint32_t DISTANCE_COUNT{2u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");

    for (uint32_t i{0u}; i < (SENSOR_COUNT * DISTANCE_COUNT); i++) {
        mock().expectOneCall("delay_ms")
            .withUnsignedIntParameter("delay_time", cfg.setup_delay_ms);

        mock().expectOneCall("reset_timer");

        mock().expectNCalls(2, "get_current_time_ms")
            .andReturnValue(0);

        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(10);
    }

    mock().expectOneCall("disable_power");

    infrared_sensors_distance_test(cfg);

    CHECK(read_ir_1_sensor_call_count > 0);
    CHECK(read_ir_2_sensor_call_count > 0);
    CHECK(read_ir_3_sensor_call_count > 0);
    CHECK(read_ir_4_sensor_call_count > 0);
}

TEST(DeviceSelfTestsTests, IrSensorsFreeReadingTestCallsFunctions)
{
    ir_free_reading_test_config cfg = {0};
    cfg.time_per_sensor_ms = 10u;
    cfg.setup_delay_ms = 500u;

    constexpr uint32_t SENSOR_COUNT{4u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");

    for (uint32_t s{0u}; s < SENSOR_COUNT; s++) {
        mock().expectOneCall("delay_ms")
            .withUnsignedIntParameter("delay_time", cfg.setup_delay_ms);

        mock().expectOneCall("reset_timer");

        /* capture start time */
        mock().expectNCalls(2, "get_current_time_ms")
            .andReturnValue(0);

        /* exit timed loop */
        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(cfg.time_per_sensor_ms);
    }

    mock().expectOneCall("disable_power");

    infrared_sensors_free_reading_test(cfg);

    CHECK(read_ir_1_sensor_call_count > 0);
    CHECK(read_ir_2_sensor_call_count > 0);
    CHECK(read_ir_3_sensor_call_count > 0);
    CHECK(read_ir_4_sensor_call_count > 0);
}

TEST(DeviceSelfTestsTests, IrSensorsReadSpeedTestCallsFunctions)
{
    constexpr uint32_t SENSOR_COUNT{4u};
    constexpr uint32_t TIME_PER_SENSOR_MS{10u};

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");

    for (uint32_t s{0u}; s < SENSOR_COUNT; s++) {
        mock().expectOneCall("reset_timer");

        /* capture start time */
        mock().expectNCalls(2, "get_current_time_ms")
            .andReturnValue(0);

        /* exit timed loop */
        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(TIME_PER_SENSOR_MS);
    }

    mock().expectOneCall("disable_power");

    infrared_sensors_read_speed_test(TIME_PER_SENSOR_MS);

    CHECK(read_ir_1_sensor_call_count > 0);
    CHECK(read_ir_2_sensor_call_count > 0);
    CHECK(read_ir_3_sensor_call_count > 0);
    CHECK(read_ir_4_sensor_call_count > 0);
}

TEST(DeviceSelfTestsTests, MoveUntilEncoderCountReachesTarget)
{
    move_until_encoder_count_config cfg = {0};

    cfg.encoder_target = 50;
    cfg.speed = 100u;
    cfg.timeout_ms = 2000u;
    cfg.drift_delay_ms = 500u;
    cfg.set_speed = set_wheel_motor_1_speed;
    cfg.get_ticks = get_encoder_1_ticks;
    cfg.clear_ticks = clear_1_encoder_ticks;

    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(0);

    for (int32_t i{0}; i < (cfg.encoder_target / 2); i++) {
        mock().expectOneCall("set_wheel_motor_1_speed")
            .withUnsignedIntParameter("speed", cfg.speed);
        mock().expectOneCall("get_current_time_ms")
            .andReturnValue(0);
    }

    mock().expectOneCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", 0u);
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(0);

    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", cfg.drift_delay_ms);

    move_until_encoder_count(cfg);
}

TEST(DeviceSelfTestsTests, MoveUntilEncoderCountBreaksOnTimeout)
{
    move_until_encoder_count_config cfg = {0};

    cfg.encoder_target = 50;
    cfg.speed = 100u;
    cfg.timeout_ms = 2000u;
    cfg.drift_delay_ms = 500u;
    cfg.set_speed = set_wheel_motor_1_speed;
    cfg.get_ticks = fake_get_ticks_no_movement;
    cfg.clear_ticks = clear_1_encoder_ticks;

    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(0);

    mock().expectOneCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", cfg.speed);
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(3000);

    mock().expectOneCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", 0u);
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(3000);

    mock().expectOneCall("delay_ms")
        .withUnsignedIntParameter("delay_time", cfg.drift_delay_ms);

    move_until_encoder_count(cfg);
}

TEST(DeviceSelfTestsTests, WheelMotorAndEncoderTestCallsFunctions)
{
    wheel_motor_and_encoder_test_config cfg = {0};

    cfg.timeout_ms = 2000u;
    cfg.drift_delay_ms = 500u;
    cfg.encoder_target = 50;
    cfg.start_speed = 100u;
    cfg.end_speed = 150u;
    cfg.speed_step = 2u;

    mock().expectOneCall("enable_power");
    mock().expectOneCall("start_timer");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectNCalls(((cfg.end_speed - cfg.start_speed) / cfg.speed_step) + 1, "reset_timer");

    mock().expectOneCall("set_wheel_motor_2_direction_forward");
    mock().expectNCalls(((cfg.end_speed - cfg.start_speed) / cfg.speed_step) + 1, "reset_timer");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectNCalls(((cfg.end_speed - cfg.start_speed) / cfg.speed_step) + 1, "reset_timer");

    mock().expectOneCall("set_wheel_motor_2_direction_backward");
    mock().expectNCalls(((cfg.end_speed - cfg.start_speed) / cfg.speed_step) + 1, "reset_timer");

    mock().expectOneCall("disable_power");

    mock().ignoreOtherCalls();
    wheel_motor_and_encoder_test(cfg);
}

TEST(DeviceSelfTestsTests, VacuumTestCallsFunctions)
{
    mock().expectOneCall("enable_power");
    for (uint8_t motor_speed = 0u; motor_speed < 255; motor_speed++) {
        mock().expectOneCall("set_vacuum_speed");
        mock().expectOneCall("delay_ms")
            .withUnsignedIntParameter("delay_time", 2000);
    }
    mock().expectOneCall("disable_power");

    vacuum_test();
}
