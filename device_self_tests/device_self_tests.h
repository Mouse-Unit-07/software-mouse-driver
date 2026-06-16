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
struct ir_distance_test_config {
    uint32_t start_distance_cm;
    uint32_t end_distance_cm;
    uint32_t trials_per_distance;
    uint32_t time_per_trial_ms;
    uint32_t setup_delay_ms;
};

struct ir_free_reading_test_config {
    uint32_t time_per_sensor_ms;
    uint32_t setup_delay_ms;
};

struct move_until_encoder_count_config {
    uint32_t timeout_ms;
    uint32_t drift_delay_ms;
    int32_t encoder_target;
    uint8_t speed;
    void (*set_speed)(uint8_t speed);
    int32_t (*get_ticks)(void);
    void (*clear_ticks)(void);
};

struct wheel_motor_and_encoder_test_config {
    uint32_t timeout_ms;
    uint32_t drift_delay_ms;
    int32_t encoder_target;
    uint8_t start_speed;
    uint8_t end_speed;
    uint8_t speed_step;
};

struct wheel_motor_deceleration_test_config {
    uint32_t timeout_ms;
    uint32_t drift_delay_ms;
    int32_t encoder_target;
    uint8_t start_speed;
    uint8_t top_speed;
    uint8_t max_accel_decel_percent;
};

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
void infrared_sensors_distance_test(struct ir_distance_test_config cfg);
void infrared_sensors_free_reading_test(struct ir_free_reading_test_config cfg);
void infrared_sensors_read_speed_test(uint32_t time_per_sensor_ms);
void wheel_motor_and_encoder_test(struct wheel_motor_and_encoder_test_config cfg);
void wheel_motor_drift_test(struct wheel_motor_and_encoder_test_config cfg);
void wheel_motor_deceleration_test(struct wheel_motor_deceleration_test_config cfg);
void vacuum_test(void);

/* helpers exposed for testing */
uint32_t measure_average_reading(uint32_t measurement_time_ms, uint32_t (*read_sensor)(void));
void move_until_encoder_count(struct move_until_encoder_count_config cfg);
void move_with_accel_decel_profile(struct move_until_encoder_count_config cfg, uint8_t start_speed,
                                   uint8_t top_speed, uint32_t accel_decel_percent);

#endif /* DEVICE_SELF_TESTS_H_ */
