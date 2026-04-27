/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : device_self_tests.h                                   */
/*                                                                            */
/* Interface for device_self_tests library                                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DEVICE_SELF_TESTS_H_
#define DEVICE_SELF_TESTS_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_device_self_tests(void);
void deinit_device_self_tests(void);
void processor_test(void);
void battery_comparator_test(void);
void power_enabler_test(void);
void led_test(void);

#endif /* DEVICE_SELF_TESTS_H_ */
