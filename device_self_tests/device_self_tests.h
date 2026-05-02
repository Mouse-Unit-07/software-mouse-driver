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
void pushbutton_test(void);
void infrared_sensors_distance_test(uint32_t trials_per_distance);
void infrared_sensors_free_reading_test(void);
void infrared_sensors_read_speed_test(void);

/* helper exposed for testing */
void move_until_encoder_count(int32_t encoder_count, uint8_t speed, void (*set_speed)(uint8_t),
                              int32_t (*get_ticks)(void), void (*clear_ticks)(void));

void wheel_motor_and_encoder_test(void);

#endif /* DEVICE_SELF_TESTS_H_ */
