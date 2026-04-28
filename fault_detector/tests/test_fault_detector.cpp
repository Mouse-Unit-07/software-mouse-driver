/*================================ FILE INFO =================================*/
/* Filename           : test_fault_detector.cpp                               */
/*                                                                            */
/* Test implementation for fault_detector.c                                   */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include "fault_detector.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

bool is_battery_low(void)
{
    return mock().actualCall("is_battery_low")
        .returnBoolValue();
}

void set_error_handler(void (*handler)(void))
{
    CHECK(handler != nullptr);
    mock().actualCall("set_error_handler");
}

void set_warning_handler(void (*handler)(void))
{
    CHECK(handler != nullptr);
    mock().actualCall("set_warning_handler");
}

void printf_call_counts(void)
{
    mock().actualCall("printf_call_counts");
}

void printf_error_log(void)
{
    mock().actualCall("printf_error_log");
}

void printf_first_runtime_error_entry(void)
{
    mock().actualCall("printf_first_runtime_error_entry");
}

void printf_warning_log(void)
{
    mock().actualCall("printf_warning_log");
}

void printf_telemetry_log(void)
{
    mock().actualCall("printf_telemetry_log");
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
void init_fault_detector_without_cpputest_checks(void)
{
    mock().ignoreOtherCalls();
    init_fault_detector();
    mock().clear();
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(FaultDetectorTests)
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
TEST(FaultDetectorTests, InitCallsFunctions)
{
    mock().expectOneCall("set_error_handler");
    mock().expectOneCall("set_warning_handler");

    init_fault_detector();
}

TEST(FaultDetectorTests, IsThereHardwareFaultChecksDevices)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    CHECK_FALSE(is_there_hardware_fault());
}

TEST(FaultDetectorTests, InitResetsFault)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(true);
    CHECK(is_there_hardware_fault());

    init_fault_detector_without_cpputest_checks();

    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    CHECK_FALSE(is_there_hardware_fault());
}

TEST(FaultDetectorTests, DeinitResetsFault)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(true);
    CHECK(is_there_hardware_fault());

    deinit_fault_detector();

    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    CHECK_FALSE(is_there_hardware_fault());
}

TEST(FaultDetectorTests, PrintHardwareStateCallsFunctions)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("printf_call_counts");
    mock().expectOneCall("printf_error_log");
    mock().expectOneCall("printf_first_runtime_error_entry");
    mock().expectOneCall("printf_warning_log");
    mock().expectOneCall("printf_telemetry_log");

    print_hardware_state();
}
