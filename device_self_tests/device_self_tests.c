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

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
/* none */

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

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
