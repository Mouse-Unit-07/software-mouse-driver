/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : device_self_tests.c                                   */
/*                                                                            */
/* Implementation for device_self_tests library                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "processor.h"
#include "battery_comparator.h"
#include "power_enabler.h"
#include "led.h"
#include "pushbutton.h"
#include "infrared_sensor.h"
#include "wheel_motor.h"
#include "magnetic_encoder.h"
#include "vacuum.h"
#include "device_self_tests.h"

/*----------------------------------------------------------------------------*/
/*                           Struct, Enum, Typedefs                           */
/*----------------------------------------------------------------------------*/
struct ir_name_and_read_function {
    const char *name;
    uint32_t (*read)(void);
};

struct wheel_motor_name_and_encoder_functions {
    const char *name;
    void (*set_direction_forward)(void);
    void (*set_direction_backward)(void);
    void (*set_speed)(uint8_t speed);
    int32_t (*get_ticks)(void);
    void (*clear_ticks)(void);
};

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static int32_t abs(int32_t n);

static void run_motor_test_sweep(const struct wheel_motor_name_and_encoder_functions *motor,
                                 struct wheel_motor_and_encoder_test_config cfg,
                                 bool is_direction_forward);

static void run_motor_deceleration_test_sweep(
    const struct wheel_motor_name_and_encoder_functions *motor,
    struct wheel_motor_deceleration_test_config cfg,
    bool is_direction_forward);

/* helpers exposed for testing */
uint32_t measure_average_reading(uint32_t measurement_time_ms, uint32_t (*read_sensor)(void));
void move_until_encoder_count(struct move_until_encoder_count_config cfg);
void move_with_accel_decel_profile(struct move_until_encoder_count_config cfg, uint8_t start_speed,
                                   uint8_t top_speed, uint32_t accel_decel_percent);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
const struct ir_name_and_read_function infrared_sensors[] = {{"IR1", read_ir_1_sensor},
                                                             {"IR2", read_ir_2_sensor},
                                                             {"IR3", read_ir_3_sensor},
                                                             {"IR4", read_ir_4_sensor}};

static const uint32_t sensor_count = 4u;

const struct wheel_motor_name_and_encoder_functions wheel_motor_and_encoders[] =
{
    {
        "MOTOR AND ENCODER 1",
        set_wheel_motor_1_direction_forward,
        set_wheel_motor_1_direction_backward,
        set_wheel_motor_1_speed,
        get_encoder_1_ticks,
        clear_1_encoder_ticks
    },
    {
        "MOTOR AND ENCODER 2",
        set_wheel_motor_2_direction_forward,
        set_wheel_motor_2_direction_backward,
        set_wheel_motor_2_speed,
        get_encoder_2_ticks,
        clear_2_encoder_ticks
    }
};

static const uint32_t wheel_motor_and_encoder_count = 2u;

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_device_self_tests(void)
{
    /* nothing to init */
}

void deinit_device_self_tests(void)
{
    /* nothing to reset/clear */
}

void processor_test(void)
{
    start_timer();
    printf("time: %" PRIu32 "ms\r\n", get_current_time_ms());
    printf("Hello World\r\n");
    delay_ms(1000);
    printf("time: %" PRIu32 "ms... resetting timer\r\n", get_current_time_ms());
    reset_timer();
    printf("Konnichiwa Sekai\r\n");
    delay_us(1000000);
    printf("time: %" PRIu32 "ms\r\n", get_current_time_ms());
}

void battery_comparator_test(void)
{
    enable_power();
    printf("power enabled\r\n");
    if (is_battery_low()) {
        printf("Battery is low\r\n");
    } else {
        printf("Battery is not low\r\n");
    }
    delay_ms(2000);

    disable_power();
    printf("power disabled\r\n");
    if (is_battery_low()) {
        printf("Battery is low\r\n");
    } else {
        printf("Battery is not low\r\n");
    }
    delay_ms(2000);
}

void power_enabler_test(void)
{
    enable_power();
    printf("power enabled\r\n");
    delay_ms(10000);
    disable_power();
    printf("power disabled\r\n");
    delay_ms(10000);
}

void led_test(void)
{
    set_led_d1_enabled(true);
    set_led_d2_enabled(true);
    set_led_d3_enabled(true);
    set_led_d4_enabled(true);
    delay_ms(1000);
    set_led_d1_enabled(false);
    set_led_d2_enabled(false);
    set_led_d3_enabled(false);
    set_led_d4_enabled(false);
    delay_ms(1000);
}

void pushbutton_test(void)
{
    for (uint32_t i = 0u; i < 5; i++) {
        printf("pushbutton count: %" PRIu32 "\r\n", get_pushbutton_count());
        delay_ms(1000);
    }
    clear_pushbutton_count();
    printf("cleared pushbutton count: %" PRIu32 "\r\n", get_pushbutton_count());
}

void infrared_sensors_distance_test(struct ir_distance_test_config cfg)
{
    enable_power();
    start_timer();

    printf("reading for %" PRIu32 " seconds per trial\r\n", cfg.time_per_trial_ms / 1000u);

    for (uint32_t s = 0u; s < sensor_count; s++) {
        printf("=== %s ===\r\n", infrared_sensors[s].name);
        for (uint32_t d = cfg.start_distance_cm; d <= cfg.end_distance_cm; d++) {
            printf("place wall %" PRIu32 "cm away from the sensor\r\n", d);

            for (uint32_t t = 0u; t < cfg.trials_per_distance; t++) {
                printf("new trial- you have %" PRIu32 " seconds\r\n", cfg.setup_delay_ms / 1000);
                delay_ms(cfg.setup_delay_ms);
                printf("avg: %" PRIu32 "\r\n",
                       measure_average_reading(cfg.time_per_trial_ms, infrared_sensors[s].read));
            }
        }
        printf("\r\n");
    }

    disable_power();
}

void infrared_sensors_free_reading_test(struct ir_free_reading_test_config cfg)
{
    enable_power();
    start_timer();

    printf("reading for %" PRIu32 " seconds per sensor\r\n", cfg.time_per_sensor_ms / 1000);

    for (uint32_t i = 0u; i < sensor_count; i++) {
        printf("=== %s ===\r\n", infrared_sensors[i].name);
        printf("you have %" PRIu32 " seconds\r\n", cfg.setup_delay_ms / 1000);
        delay_ms(cfg.setup_delay_ms);

        reset_timer();
        uint32_t current_time_ms = get_current_time_ms();
        while ((get_current_time_ms() - current_time_ms) < (cfg.time_per_sensor_ms)) {
            printf("reading: %" PRIu32 "\r\n", infrared_sensors[i].read());
        }
        printf("\r\n");
    }

    disable_power();
}

void infrared_sensors_read_speed_test(uint32_t time_per_sensor_ms)
{
    enable_power();
    start_timer();

    printf("reading for %" PRIu32 " seconds per sensor\r\n", time_per_sensor_ms / 1000);

    for (uint32_t i = 0u; i < sensor_count; i++) {
        printf("=== %s ===\r\n", infrared_sensors[i].name);

        uint32_t read_count = 0u;
        reset_timer();
        uint32_t current_time_ms = get_current_time_ms();
        while ((get_current_time_ms() - current_time_ms) < (time_per_sensor_ms)) {
            infrared_sensors[i].read();
            read_count++;
        }
        printf("total read: %" PRIu32 "\r\n", read_count);
    }

    disable_power();
}

void wheel_motor_and_encoder_test(struct wheel_motor_and_encoder_test_config cfg)
{
    enable_power();
    start_timer();

    printf("encoder target for each run: %" PRId32 "\r\n", cfg.encoder_target);

    printf("moving motors forward\r\n");
    for (uint32_t i = 0u; i < wheel_motor_and_encoder_count; i++) {
        run_motor_test_sweep(&wheel_motor_and_encoders[i], cfg, true);
    }

    printf("moving motors backward\r\n");
    for (uint32_t i = 0u; i < wheel_motor_and_encoder_count; i++) {
        run_motor_test_sweep(&wheel_motor_and_encoders[i], cfg, false);
    }

    disable_power();
}

void wheel_motor_deceleration_test(struct wheel_motor_deceleration_test_config cfg)
{
    enable_power();
    start_timer();

    printf("encoder target for each run: %" PRId32 "\r\n", cfg.encoder_target);

    printf("moving motors forward\r\n");
    for (uint32_t i = 0u; i < wheel_motor_and_encoder_count; i++) {
        run_motor_deceleration_test_sweep(&wheel_motor_and_encoders[i], cfg, true);
    }

    printf("moving motors backward\r\n");
    for (uint32_t i = 0u; i < wheel_motor_and_encoder_count; i++) {
        run_motor_deceleration_test_sweep(&wheel_motor_and_encoders[i], cfg, false);
    }

    disable_power();
}

void vacuum_test(void)
{
    enable_power();

    for (uint8_t motor_speed = 0u; motor_speed < 255; motor_speed++) {
        set_vacuum_speed(motor_speed);
        printf("speed: %" PRIu8 "\r\n", motor_speed);
        delay_ms(2000);
    }

    disable_power();
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
static int32_t abs(int32_t n)
{
    return n < 0 ? -n : n;
}

static void run_motor_test_sweep(const struct wheel_motor_name_and_encoder_functions *motor,
                                 struct wheel_motor_and_encoder_test_config cfg,
                                 bool is_direction_forward)
{
    if (cfg.speed_step == 0u) {
        return;
    }

    if (is_direction_forward) {
        motor->set_direction_forward();
    } else {
        motor->set_direction_backward();
    }

    printf("%s\r\n", motor->name);

    for (uint32_t speed = cfg.start_speed; speed <= cfg.end_speed; speed += cfg.speed_step) {
        printf("speed: %" PRIu32 "\r\n", speed);
        reset_timer();

        struct move_until_encoder_count_config move_cfg = {0};
        if (is_direction_forward) {
            move_cfg.encoder_target = cfg.encoder_target;
        } else {
            move_cfg.encoder_target = -cfg.encoder_target;
        }
        move_cfg.speed = (uint8_t)speed;
        move_cfg.timeout_ms = cfg.timeout_ms;
        move_cfg.drift_delay_ms = cfg.drift_delay_ms;
        move_cfg.set_speed = motor->set_speed;
        move_cfg.get_ticks = motor->get_ticks;
        move_cfg.clear_ticks = motor->clear_ticks;

        move_until_encoder_count(move_cfg);
    }
}

static void run_motor_deceleration_test_sweep(
    const struct wheel_motor_name_and_encoder_functions *motor,
    struct wheel_motor_deceleration_test_config cfg,
    bool is_direction_forward)
{
    if (is_direction_forward) {
        motor->set_direction_forward();
    } else {
        motor->set_direction_backward();
    }

    printf("%s\r\n", motor->name);

    uint32_t max_percent = cfg.max_accel_decel_percent;
    if (max_percent > 100u) {
        max_percent = 100u;
    }

    for (uint32_t percent = 0u; percent <= max_percent; percent++) {
        printf("accel/decel ratio: %" PRIu32 "%%\r\n", percent);

        reset_timer();

        struct move_until_encoder_count_config move_cfg = {0};

        move_cfg.timeout_ms = cfg.timeout_ms;
        move_cfg.drift_delay_ms = cfg.drift_delay_ms;
        if (is_direction_forward) {
            move_cfg.encoder_target = cfg.encoder_target;
        } else {
            move_cfg.encoder_target = -cfg.encoder_target;
        }
        move_cfg.set_speed = motor->set_speed;
        move_cfg.get_ticks = motor->get_ticks;
        move_cfg.clear_ticks = motor->clear_ticks;

        move_with_accel_decel_profile(move_cfg, cfg.start_speed, cfg.top_speed, percent);
    }
}

/* exposed for testing */
uint32_t measure_average_reading(uint32_t measurement_time_ms, uint32_t (*read_sensor)(void))
{
    reset_timer();

    uint32_t start_time_ms = get_current_time_ms();

    uint64_t total_reading = 0u;
    uint32_t reading_count = 0u;

    while ((get_current_time_ms() - start_time_ms) < measurement_time_ms) {
        total_reading += read_sensor();
        reading_count++;
    }

    return total_reading / reading_count;
}

/* exposed for testing */
void move_until_encoder_count(struct move_until_encoder_count_config cfg)
{
    uint32_t start_time_ms = get_current_time_ms();
    cfg.clear_ticks();
    int32_t prev_ticks = cfg.get_ticks();

    while (abs(cfg.get_ticks()) < abs(cfg.encoder_target)) {
        cfg.set_speed(cfg.speed);
        int32_t current_ticks = cfg.get_ticks();

        if (((get_current_time_ms() - start_time_ms) > cfg.timeout_ms)
            && (current_ticks == prev_ticks)) {
            break;
        }
        prev_ticks = current_ticks;
    }

    cfg.set_speed(0u);
    uint32_t end_time_ms = get_current_time_ms();
    delay_ms(cfg.drift_delay_ms);

    printf("%" PRId32 ", %" PRIu32 "ms\r\n", cfg.get_ticks(), end_time_ms - start_time_ms);

    cfg.clear_ticks();
}

/* exposed for testing */
void move_with_accel_decel_profile(struct move_until_encoder_count_config cfg, uint8_t start_speed,
                                   uint8_t top_speed, uint32_t accel_decel_percent)
{
    uint32_t start_time_ms = get_current_time_ms();

    cfg.clear_ticks();

    int32_t prev_ticks = cfg.get_ticks();
    int32_t abs_target = abs(cfg.encoder_target);
    uint32_t accel_ticks = (abs_target * accel_decel_percent) / 200u;
    uint32_t decel_start_ticks = abs_target - accel_ticks;

    while (abs(cfg.get_ticks()) < abs_target) {
        int32_t current_ticks = abs(cfg.get_ticks());
        uint8_t speed = top_speed;

        if ((accel_ticks > 0u) && (current_ticks < (int32_t)accel_ticks)) {
            speed = start_speed + (((top_speed - start_speed) * current_ticks) / accel_ticks);
        } else if ((accel_ticks > 0u) && (current_ticks >= (int32_t)decel_start_ticks)) {
            uint32_t decel_progress = current_ticks - decel_start_ticks;
            speed = top_speed - (((top_speed - start_speed) * decel_progress) / accel_ticks);
        }
        cfg.set_speed(speed);

        int32_t latest_ticks = cfg.get_ticks();

        if (((get_current_time_ms() - start_time_ms) > cfg.timeout_ms)
            && (latest_ticks == prev_ticks)) {
            break;
        }

        prev_ticks = latest_ticks;
    }

    cfg.set_speed(0u);

    uint32_t end_time_ms = get_current_time_ms();
    delay_ms(cfg.drift_delay_ms);

    printf("%" PRId32 ", %" PRIu32 "ms\r\n", cfg.get_ticks(), end_time_ms - start_time_ms);
    cfg.clear_ticks();
}
