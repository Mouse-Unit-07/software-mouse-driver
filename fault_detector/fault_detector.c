/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : fault_detector.c                                      */
/*                                                                            */
/* Implementation for fault_detector library                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "battery_comparator.h"
#include "runtime_diagnostics.h"

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static void check_hardware_state(void);
static void runtime_warning_handler(void);
static void runtime_error_handler(void);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static bool hardware_fault_found = false;
static bool low_battery = false;

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_fault_detector(void)
{
    hardware_fault_found = false;
    low_battery = false;

    set_error_handler(runtime_error_handler);
    set_warning_handler(runtime_warning_handler);
}

void deinit_fault_detector(void)
{
    hardware_fault_found = false;
    low_battery = false;
}

bool is_there_hardware_fault(void)
{
    check_hardware_state();
    return hardware_fault_found;
}

void print_hardware_state(void)
{
    check_hardware_state();

    printf("=== hardware devices ===\r\n");
    if (low_battery) {
        printf("battery is low\r\n");
    } else {
        printf("battery is stable\r\n");
    }
    printf("\r\n");

    printf("=== runtime diagnostics ===\r\n");
    printf_call_counts();
    printf("== runtime errors ==\r\n");
    printf_error_log();
    printf("= first runtime error =\r\n");
    printf_first_runtime_error_entry();
    printf("== runtime warnings ==\r\n");
    printf_warning_log();
    printf("== runtime telemetry ==\r\n");
    printf_telemetry_log();
    printf("\r\n");
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
static void check_hardware_state(void)
{
    if (is_battery_low()) {
        low_battery = true;
        hardware_fault_found = true;
    }
}

static void runtime_warning_handler(void)
{
    printf("runtime warning logged\r\n");
}

static void runtime_error_handler(void)
{
    printf("runtime error logged\r\n");
    hardware_fault_found = true;
}
