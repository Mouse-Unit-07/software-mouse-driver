/*================================ FILE INFO =================================*/
/* Filename           : test_global_time.cpp                                  */
/*                                                                            */
/* Test implementation for global_time.c                                      */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdint.h>
#include "global_time.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <cstdint>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

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

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
/* none */

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(GlobalTimeTests)
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
TEST(GlobalTimeTests, InitStartsTimer)
{
    mock().expectOneCall("start_timer");
    init_global_time();
}

TEST(GlobalTimeTests, DeinitResetsTimer)
{
    mock().expectOneCall("reset_timer");
    deinit_global_time();
}

TEST(GlobalTimeTests, RestartResetsTimer)
{
    mock().expectOneCall("reset_timer");
    deinit_global_time();
}

TEST(GlobalTimeTests, GetCurrentTimeSecFetchesTimeAndDividesMs)
{
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(5000u);
    CHECK(get_current_global_time_sec() == 5u);
}

TEST(GlobalTimeTests, GetElapsedTimeSecReturnsDifference)
{
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(5000u);
    CHECK(get_elapsed_global_time_sec(2u) == 3u);
}

TEST(GlobalTimeTests, GetElapsedTimeSecDoesNotReturnNegativeValues)
{
    mock().expectOneCall("get_current_time_ms")
        .andReturnValue(3000u);
    CHECK(get_elapsed_global_time_sec(5000) == 0u);
}

TEST(GlobalTimeTests, DelayMsCallsFunction)
{
    mock().expectOneCall("delay_ms")
        .withParameter("delay_time", 1000);
    delay_global_time_ms(1000);
}

TEST(GlobalTimeTests, DelayUsCallsFunction)
{
    mock().expectOneCall("delay_us")
        .withParameter("delay_time", 1000);
    delay_global_time_us(1000);
}
