/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : device_self_tests.c                                   */
/*                                                                            */
/* Implementation for device_self_tests library                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "processor.h"
#include "battery_comparator.h"
#include "power_enabler.h"
#include "led.h"
#include "pushbutton.h"
#include "infrared_sensor.h"
#include "wheel_motor.h"
#include "magnetic_encoder.h"
#include "vacuum.h"

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static int32_t abs(int32_t n);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static uint32_t (*read_functions[])(void) = {read_ir_1_sensor, 
                                             read_ir_2_sensor, 
                                             read_ir_3_sensor, 
                                             read_ir_4_sensor};
static const uint32_t sensor_count = 4u;

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_device_self_tests(void)
{
    /* nothing to init */
}

void deinit_device_self_tests(void)
{
    /* nothing to reset/clear */
}

void processor_test(void)
{
    start_timer();
    printf("time: %" PRIu32 "ms\r\n", get_current_time_ms());
    printf("Hello World\r\n");
    delay_ms(1000);
    printf("time: %" PRIu32 "ms... resetting timer\r\n", get_current_time_ms());
    reset_timer();
    printf("Konnichiwa Sekai\r\n");
    delay_us(1000000);
    printf("time: %" PRIu32 "ms\r\n", get_current_time_ms());
}

void battery_comparator_test(void)
{
    enable_power();
    printf("power enabled\r\n");
    if (is_battery_low()) {
        printf("Battery is low\r\n");
    } else {
        printf("Battery is not low\r\n");
    }
    delay_ms(2000);

    disable_power();
    printf("power disabled\r\n");
    if (is_battery_low()) {
        printf("Battery is low\r\n");
    } else {
        printf("Battery is not low\r\n");
    }
    delay_ms(2000);
}

void power_enabler_test(void)
{
    enable_power();
    printf("power enabled\r\n");
    delay_ms(10000);
    disable_power();
    printf("power disabled\r\n");
    delay_ms(10000);
}

void led_test(void)
{
    set_led_d1_enabled(true);
    set_led_d2_enabled(true);
    set_led_d3_enabled(true);
    set_led_d4_enabled(true);
    delay_ms(1000);
    set_led_d1_enabled(false);
    set_led_d2_enabled(false);
    set_led_d3_enabled(false);
    set_led_d4_enabled(false);
    delay_ms(1000);
}

void pushbutton_test(void)
{
    for (uint32_t i = 0u; i < 5; i++) {
        printf("pushbutton count: %" PRIu32 "\r\n", get_pushbutton_count());
        delay_ms(1000);
    }
    clear_pushbutton_count();
    printf("cleared pushbutton count: %" PRIu32 "\r\n", get_pushbutton_count());
}

void infrared_sensors_distance_test(uint32_t trials_per_distance)
{
    enable_power();
    start_timer();

    const uint32_t START_DISTANCE_CM = 2u;
    const uint32_t END_DISTANCE_CM = 15u;
    const uint32_t TIME_PER_TRIAL_SEC = 3u;
    const uint32_t DELAY_TIME_MS = 5000u;
    printf("reading for %" PRIu32 " seconds per trial\r\n", TIME_PER_TRIAL_SEC);
    printf("you have %" PRIu32 " seconds to prepare before each sensor trial starts\r\n",
           DELAY_TIME_MS / 1000);

    for (uint32_t s = 0u; s < sensor_count; s++) {
        printf("=== IR sensor %" PRIu32 "===\r\n", s + 1);
        for (uint32_t d = START_DISTANCE_CM; d <= END_DISTANCE_CM; d++) {
            printf("place wall %" PRIu32 "cm away from the sensor\r\n", d);

            for (uint32_t t = 0u; t < trials_per_distance; t++) {
                delay_ms(DELAY_TIME_MS);
                reset_timer();
                uint32_t current_time_ms = get_current_time_ms();
                uint64_t total_reading = 0u;
                uint32_t reading_count = 0u;
                while ((get_current_time_ms() - current_time_ms) < (TIME_PER_TRIAL_SEC * 1000)) {
                    total_reading += read_functions[s]();
                    reading_count++;
                }
                printf("%" PRIu64 "\r\n", total_reading / reading_count);
            }
        }
        printf("\r\n");
    }

    disable_power();
}

void infrared_sensors_free_reading_test(void)
{
    enable_power();
    start_timer();

    const uint32_t TIME_PER_SENSOR_SEC = 30u;
    const uint32_t DELAY_TIME_MS = 5000u;
    printf("reading for %" PRIu32 " seconds per sensor\r\n", TIME_PER_SENSOR_SEC);
    printf("you have %" PRIu32 " seconds to prepare before each sensor test starts\r\n",
           DELAY_TIME_MS / 1000);

    for (uint32_t i = 0u; i < sensor_count; i++) {
        printf("=== IR sensor %" PRIu32 "===\r\n", i + 1);
        delay_ms(DELAY_TIME_MS);

        reset_timer();
        uint32_t current_time_ms = get_current_time_ms();
        while ((get_current_time_ms() - current_time_ms) < (TIME_PER_SENSOR_SEC * 1000)) {
            printf("reading: %" PRIu32 "\r\n", read_functions[i]());
        }
        printf("\r\n");
    }

    disable_power();
}

void infrared_sensors_read_speed_test(void)
{
    enable_power();
    start_timer();

    const uint32_t TIME_PER_SENSOR_SEC = 5u;
    printf("reading for %" PRIu32 " seconds per sensor\r\n", TIME_PER_SENSOR_SEC);

    for (uint32_t i = 0u; i < sensor_count; i++) {
        printf("=== IR sensor %" PRIu32 "===\r\n", i + 1);

        uint32_t read_count = 0u;
        reset_timer();
        uint32_t current_time_ms = get_current_time_ms();
        while ((get_current_time_ms() - current_time_ms) < (TIME_PER_SENSOR_SEC * 1000)) {
            read_functions[i]();
            read_count++;
        }
        printf("total read: %" PRIu32 "\r\n", read_count);
    }

    disable_power();
}

void move_until_encoder_count(int32_t encoder_count, uint8_t speed, void (*set_speed)(uint8_t),
                              int32_t (*get_ticks)(void), void (*clear_ticks)(void))
{
    const uint32_t TIME_LIMIT_MS = 2000u;
    const uint32_t DRIFT_TIME_MS = 500u;

    uint32_t start_time_ms = get_current_time_ms();;
    clear_ticks();
    uint32_t prev_ticks = get_ticks();
    while (abs(get_ticks()) < abs(encoder_count)) {
        set_speed(speed);
        uint32_t current_ticks = get_ticks();
        if (((get_current_time_ms() - start_time_ms) > TIME_LIMIT_MS)
            && (current_ticks == prev_ticks)) {
            break;
        }
        prev_ticks = current_ticks;
    }
    set_speed(0u);
    uint32_t end_time_ms = get_current_time_ms();
    delay_ms(DRIFT_TIME_MS);
    printf("%" PRId32 ", %" PRIu32 "ms\r\n", get_ticks(), end_time_ms - start_time_ms);
    clear_ticks();
}

void wheel_motor_and_encoder_test(void)
{
    enable_power();
    start_timer();

    const int32_t ENCODER_TARGET = 100;
    printf("encoder target for each run: %" PRId32 "\r\n", ENCODER_TARGET);

    set_wheel_motor_1_direction_forward();
    set_wheel_motor_2_direction_forward();
    printf("moving motors forward\r\n");
    for (uint8_t motor_speed = 40u; motor_speed < 255u; motor_speed++) {
        reset_timer();
        printf("speed: %" PRIu8 "\r\n", motor_speed);

        printf("m1: ");
        move_until_encoder_count(ENCODER_TARGET, motor_speed, set_wheel_motor_1_speed,
                                 get_encoder_1_ticks, clear_1_encoder_ticks);

        printf("m2: ");
        move_until_encoder_count(ENCODER_TARGET, motor_speed, set_wheel_motor_2_speed,
                                 get_encoder_2_ticks, clear_2_encoder_ticks);
    }

    set_wheel_motor_1_direction_backward();
    set_wheel_motor_2_direction_backward();
    printf("moving motors backward\r\n");
    for (uint8_t motor_speed = 40u; motor_speed < 255u; motor_speed++) {
        reset_timer();
        printf("speed: %" PRIu8 "\r\n", motor_speed);

        printf("m1: ");
        move_until_encoder_count(-ENCODER_TARGET, motor_speed, set_wheel_motor_1_speed,
                                 get_encoder_1_ticks, clear_1_encoder_ticks);

        printf("m2: ");
        move_until_encoder_count(-ENCODER_TARGET, motor_speed, set_wheel_motor_2_speed,
                                 get_encoder_2_ticks, clear_2_encoder_ticks);
    }

    disable_power();
    delay_ms(10000);
}

void vacuum_test(void)
{
    enable_power();

    for (uint8_t motor_speed = 0u; motor_speed < 255; motor_speed++) {
        set_vacuum_speed(motor_speed);
        printf("speed: %" PRIu8 "\r\n", motor_speed);
        delay_ms(2000);
    }

    disable_power();
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
static int32_t abs(int32_t n)
{
    return n < 0 ? -n : n;
}

