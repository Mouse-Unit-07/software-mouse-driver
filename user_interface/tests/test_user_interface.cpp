/*================================ FILE INFO =================================*/
/* Filename           : test_user_interface.cpp                               */
/*                                                                            */
/* Test implementation for user_interface.c                                   */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdint.h>
#include "user_interface.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

uint32_t get_pushbutton_count(void)
{
    return mock().actualCall("get_pushbutton_count")
        .returnUnsignedIntValue();
}

void clear_pushbutton_count(void)
{
    mock().actualCall("clear_pushbutton_count");
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
/* none */

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(UserRequestTests)
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
TEST(UserRequestTests, InitUserRequest)
{
    init_user_interface();
}

TEST(UserRequestTests, DeinitUserRequest)
{
    deinit_user_interface();
}

TEST(UserRequestTests, GetUserRequestCallsGetPushbuttonCount)
{
    mock().expectOneCall("get_pushbutton_count")
        .andReturnValue(4);
    CHECK(get_button_press_count() == 4);
}

TEST(UserRequestTests, ClearUserRequestCallsClearPushbuttonCount)
{
    mock().expectOneCall("clear_pushbutton_count");
    clear_button_press_count();
}
