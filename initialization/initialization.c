/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : initialization.c                                      */
/*                                                                            */
/* Implementation for initialization library                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "processor.h"
#include "battery_comparator.h"
#include "led.h"
#include "power_enabler.h"
#include "pushbutton.h"
#include "infrared_sensor.h"
#include "magnetic_encoder.h"
#include "wheel_motor.h"
#include "vacuum.h"

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
void init_mouse(void)
{
    init_processor();
    init_battery_comparator();
    init_leds();
    init_power_enabler();
    init_pushbutton();
    init_infrared_sensors();
    init_magnetic_encoders();
    init_wheel_motors();
    init_vacuum();
}

void deinit_mouse(void)
{
    deinit_vacuum();
    deinit_wheel_motors();
    deinit_magnetic_encoders();
    deinit_infrared_sensors();
    deinit_pushbutton();
    deinit_power_enabler();
    deinit_leds();
    deinit_battery_comparator();
    deinit_processor();
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
