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
    return mock().actualCall("get_pushbutton_count").returnUnsignedIntValue();
}

void clear_pushbutton_count(void)
{
    mock().actualCall("clear_pushbutton_count");
}

bool is_usart_rx_buffer_empty(void)
{
    return mock().actualCall("is_usart_rx_buffer_empty").returnBoolValue();
}

bool is_usart_rx_buffer_full(void)
{
    return mock().actualCall("is_usart_rx_buffer_full").returnBoolValue();
}

char pop_usart_rx_buffer(void)
{
    return (char)(mock().actualCall("pop_usart_rx_buffer").returnIntValue());
}

void clear_usart_rx_buffer(void)
{
    mock().actualCall("clear_usart_rx_buffer");
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

TEST(UserRequestTests, GetUserRequestCallsFunctions)
{
    mock().expectOneCall("get_pushbutton_count").andReturnValue(4);
    CHECK(get_button_press_count() == 4);
}

TEST(UserRequestTests, ClearUserRequestCallsFunctions)
{
    mock().expectOneCall("clear_pushbutton_count");
    clear_button_press_count();
}

TEST(UserRequestTests, IsCliBufferEmptyCallsFunctions)
{
    mock().expectOneCall("is_usart_rx_buffer_empty").andReturnValue(false);
    is_cli_buffer_empty();
}

TEST(UserRequestTests, IsCliBufferFullCallsFunctions)
{
    mock().expectOneCall("is_usart_rx_buffer_full").andReturnValue(false);
    is_cli_buffer_full();
}

TEST(UserRequestTests, PopCliBufferCallsFunctions)
{
    mock().expectOneCall("pop_usart_rx_buffer").andReturnValue('\0');
    pop_cli_buffer();
}

TEST(UserRequestTests, ClearCliBufferCallsFunctions)
{
    mock().expectOneCall("clear_usart_rx_buffer");
    clear_cli_buffer();
}
