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

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
/* none */

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
    printf("Hello World\r\n");
    delay_ms(1000);
    printf("Konnichiwa Sekai\r\n");
    delay_us(1000000);
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

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
