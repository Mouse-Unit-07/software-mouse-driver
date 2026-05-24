/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : user_interface.c                                      */
/*                                                                            */
/* Implementation for user_interface library                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "pushbutton.h"
#include "processor.h"

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
void init_user_interface(void)
{
    /* nothing to init */
}

void deinit_user_interface(void)
{
    /* nothing to clear/reset */
}

uint32_t get_button_press_count(void)
{
    return get_pushbutton_count();
}

void clear_button_press_count(void)
{
    clear_pushbutton_count();
}

bool is_cli_buffer_empty(void)
{
    return is_usart_rx_buffer_empty();
}

bool is_cli_buffer_full(void)
{
    return is_usart_rx_buffer_full();
}

char pop_cli_buffer(void)
{
    return pop_usart_rx_buffer();
}

void clear_cli_buffer(void)
{
    clear_usart_rx_buffer();
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
