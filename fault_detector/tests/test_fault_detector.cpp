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

bool is_current_limit_detection_asserted(void)
{
    return mock().actualCall("is_current_limit_detection_asserted")
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
    mock().disable();
    init_fault_detector();
    mock().enable();
}

void set_device_mock_expectations(bool battery_fault, bool cld_fault)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(battery_fault);
    mock().expectOneCall("is_current_limit_detection_asserted")
        .andReturnValue(cld_fault);
}

bool is_there_hardware_fault_without_cpputest_checks(void)
{
    mock().disable();
    bool result = is_there_hardware_fault();
    mock().enable();

    return result;
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

TEST(FaultDetectorTests, IsThereHardwareFaultReturnsTrueWhenBatteryLow)
{
    set_device_mock_expectations(true, false);
    CHECK(is_there_hardware_fault());
}

TEST(FaultDetectorTests, IsThereHardwareFaultReturnsTrueWhenCldAsserted)
{
    set_device_mock_expectations(false, true);
    CHECK(is_there_hardware_fault());
}

TEST(FaultDetectorTests, IsThereHardwareFaultReturnsFalseWhenAllClear)
{
    set_device_mock_expectations(false, false);
    CHECK_FALSE(is_there_hardware_fault());
}

TEST(FaultDetectorTests, InitResetsFault)
{
    set_device_mock_expectations(true, true);
    CHECK(is_there_hardware_fault());

    init_fault_detector_without_cpputest_checks();

    CHECK_FALSE(is_there_hardware_fault_without_cpputest_checks());
}

TEST(FaultDetectorTests, DeinitResetsFault)
{
    set_device_mock_expectations(true, true);
    CHECK(is_there_hardware_fault());

    deinit_fault_detector();

    CHECK_FALSE(is_there_hardware_fault_without_cpputest_checks());
}

TEST(FaultDetectorTests, PrintHardwareStateCallsFunctions)
{
    mock().expectOneCall("is_battery_low")
        .andReturnValue(false);
    mock().expectOneCall("is_current_limit_detection_asserted")
        .andReturnValue(false);
    mock().expectOneCall("printf_call_counts");
    mock().expectOneCall("printf_error_log");
    mock().expectOneCall("printf_first_runtime_error_entry");
    mock().expectOneCall("printf_warning_log");
    mock().expectOneCall("printf_telemetry_log");

    print_hardware_state();
}
