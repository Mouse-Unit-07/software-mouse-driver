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
void init_processor(void)
{
    mock().actualCall("init_processor");
}

void deinit_processor(void)
{
    mock().actualCall("deinit_processor");
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
    init_mouse();
}

TEST(InitializationTests, DeinitCallsFunctions)
{
    mock().expectOneCall("deinit_processor");
    deinit_mouse();
}
