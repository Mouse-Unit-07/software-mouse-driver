/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : global_time.c                                         */
/*                                                                            */
/* Implementation for global_time library                                     */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdint.h>
#include "processor.h"
#include "global_time.h"

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
void init_global_time(void)
{
    start_timer();
}

void deinit_global_time(void)
{
    reset_timer();
}

void restart_global_time(void)
{
    reset_timer();
}

uint32_t get_current_global_time_sec(void)
{
    return get_current_time_ms() / 1000;
}

uint32_t get_elapsed_global_time_sec(uint32_t start_time_sec)
{
    uint32_t current_time_sec = get_current_time_ms() / 1000;
    if (current_time_sec <= start_time_sec) {
        return 0u;
    }
    
    return current_time_sec - start_time_sec;
}

void delay_global_time_ms(uint32_t delay_time)
{
    delay_ms(delay_time);
}

void delay_global_time_us(uint32_t delay_time)
{
    delay_us(delay_time);
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
/* none */
