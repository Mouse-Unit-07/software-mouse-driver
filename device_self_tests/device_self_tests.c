/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : device_self_tests.c                                   */
/*                                                                            */
/* Implementation for device_self_tests library                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "processor.h"
#include "battery_comparator.h"
#include "power_enabler.h"

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

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
