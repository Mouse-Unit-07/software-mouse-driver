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
#include "processor.h"
#include "device_self_tests.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <array>
#include <cstdio>
#include <fstream>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

void delay_ms(uint32_t delay_time)
{
    mock().actualCall("delay_ms");
}

void delay_us(uint32_t delay_time)
{
    mock().actualCall("delay_us");
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
FILE *standard_output{nullptr};
constexpr const char *TEST_FILE{"test_output.txt"};

void redirect_stdout_to_file(void)
{
    standard_output = stdout;
    CHECK(freopen(TEST_FILE, "w+", stdout) != nullptr);
}

void check_printf_output(const char *expected_string)
{
    constexpr std::size_t MAX_BUFFER_SIZE{128};
    std::array<char, MAX_BUFFER_SIZE> buffer{};

    FILE* file{fopen(TEST_FILE, "r")};
    CHECK(file != nullptr);
    
    const size_t bytes_read{fread(buffer.data(), 1, buffer.size() - 1, file)};
    CHECK(bytes_read > 0);
    fclose(file);

    STRCMP_EQUAL(expected_string, buffer.data());
}

void restore_stdout(void)
{
    CHECK(stdout != nullptr);
    fclose(stdout);
    CHECK(freopen("CON", "w", standard_output) != nullptr);
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(DeviceSelfTestsTests)
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
TEST(DeviceSelfTestsTests, InitDeviceSelfTests)
{
    init_device_self_tests();
}

TEST(DeviceSelfTestsTests, DeinitDeviceSelfTests)
{
    deinit_device_self_tests();
}

TEST(DeviceSelfTestsTests, ProcessorTestsPrintsHelloWorldAndDelays)
{
    mock().expectOneCall("delay_ms");
    mock().expectOneCall("delay_us");

    redirect_stdout_to_file();
    processor_test();
    fflush(stdout);
    restore_stdout();
    check_printf_output("Hello World\r\nKonnichiwa Sekai\r\n");
}
