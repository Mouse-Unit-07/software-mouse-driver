/*================================ FILE INFO =================================*/
/* Filename           : test_initialization.cpp                               */
/*                                                                            */
/* Test implementation for initialization.c                                   */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdint.h>
#include "processor.h"
#include "initialization.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

void init_processor(void)
{
    mock().actualCall("init_processor");
}

void init_battery_comparator(void)
{
    mock().actualCall("init_battery_comparator");
}

void init_leds(void)
{
    mock().actualCall("init_leds");
}

void init_power_enabler(void)
{
    mock().actualCall("init_power_enabler");
}

void init_pushbutton(void)
{
    mock().actualCall("init_pushbutton");
}

void init_infrared_sensors(void)
{
    mock().actualCall("init_infrared_sensors");
}

void init_magnetic_encoders(void)
{
    mock().actualCall("init_magnetic_encoders");
}

void init_vacuum(void)
{
    mock().actualCall("init_vacuum");
}

void deinit_processor(void)
{
    mock().actualCall("deinit_processor");
}

void deinit_battery_comparator(void)
{
    mock().actualCall("deinit_battery_comparator");
}

void deinit_leds(void)
{
    mock().actualCall("deinit_leds");
}

void deinit_power_enabler(void)
{
    mock().actualCall("deinit_power_enabler");
}

void deinit_pushbutton(void)
{
    mock().actualCall("deinit_pushbutton");
}

void deinit_infrared_sensors(void)
{
    mock().actualCall("deinit_infrared_sensors");
}

void deinit_magnetic_encoders(void)
{
    mock().actualCall("deinit_magnetic_encoders");
}

void deinit_vacuum(void)
{
    mock().actualCall("deinit_vacuum");
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
/* none */

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(InitializationTests)
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
TEST(InitializationTests, InitCallsFunctions)
{
    mock().expectOneCall("init_processor");
    mock().expectOneCall("init_battery_comparator");
    mock().expectOneCall("init_leds");
    mock().expectOneCall("init_power_enabler");
    mock().expectOneCall("init_pushbutton");
    mock().expectOneCall("init_infrared_sensors");
    mock().expectOneCall("init_magnetic_encoders");
    mock().expectOneCall("init_vacuum");
    init_mouse();
}

TEST(InitializationTests, DeinitCallsFunctions)
{
    mock().expectOneCall("deinit_vacuum");
    mock().expectOneCall("deinit_magnetic_encoders");
    mock().expectOneCall("deinit_infrared_sensors");
    mock().expectOneCall("deinit_pushbutton");
    mock().expectOneCall("deinit_power_enabler");
    mock().expectOneCall("deinit_leds");
    mock().expectOneCall("deinit_battery_comparator");
    mock().expectOneCall("deinit_processor");
    deinit_mouse();
}
