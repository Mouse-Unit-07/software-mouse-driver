/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : navigation.c                                          */
/*                                                                            */
/* Implementation for navigation library                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "infrared_sensor.h"
#include "wheel_motor.h"
#include "magnetic_encoder.h"
#include "navigation.h"

/*----------------------------------------------------------------------------*/
/*                           Struct, Enum, Typedefs                           */
/*----------------------------------------------------------------------------*/
struct maze_calculated_params {
    double cell_size_mm;
};

struct mouse_calculated_params {
    double gear_ratio;
    double encoder_ticks_per_revolution;
    double encoder_ticks_per_millimeter;
    double encoder_ticks_per_rotation_radian;
};

struct navigation_params {
    int32_t move_forward_one_cell_target_ticks;
};

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static int32_t clamp_int32(int32_t val, int32_t min, int32_t max);

/* exposed for testing */
void init_move_forward_state(struct move_forward_state *state);
bool is_move_forward_complete(void);
struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         struct move_forward_config cfg);
struct motor_output calculate_move_forward_motor_output(struct move_forward_errors errors,
                                                        struct move_forward_control_config cfg);

void apply_motor_output(struct motor_output output);
void stop_motors(void);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static struct mouse_calculated_params mouse_params = {0};
static struct maze_calculated_params maze_params = {0};
static struct navigation_params navigation_params = {0};

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_navigation(void)
{
    memset(&mouse_params, 0, sizeof(mouse_params));
    memset(&maze_params, 0, sizeof(maze_params));
    memset(&navigation_params, 0, sizeof(navigation_params));
}

void deinit_navigation(void)
{
    memset(&mouse_params, 0, sizeof(mouse_params));
    memset(&maze_params, 0, sizeof(maze_params));
    memset(&navigation_params, 0, sizeof(navigation_params));
}

void calculate_mouse_params(struct mouse_physical_params p)
{
    mouse_params.gear_ratio = p.motor_pinion_gear_teeth / p.wheel_gear_teeth;

    mouse_params.encoder_ticks_per_revolution =
        (p.encoder_events_per_revolution / 4.0) * (1.0 / mouse_params.gear_ratio);

    mouse_params.encoder_ticks_per_millimeter =
        mouse_params.encoder_ticks_per_revolution / (M_PI * p.wheel_diameter_mm);

    mouse_params.encoder_ticks_per_rotation_radian =
        (mouse_params.encoder_ticks_per_revolution * p.wheel_base_mm)
        / (2.0 * M_PI * p.wheel_diameter_mm);
}

void calculate_maze_params(struct maze_physical_params p)
{
    maze_params.cell_size_mm = p.post_size_mm + p.wall_size_mm;
}

void calculate_navigation_params(void)
{
    navigation_params.move_forward_one_cell_target_ticks =
        (int32_t)(maze_params.cell_size_mm * mouse_params.encoder_ticks_per_millimeter);
}

void move_forward(struct move_forward_config cfg)
{
    struct move_forward_state state = {0};

    init_move_forward_state(&state);

    const uint32_t MAX_STEPS = 10000u;
    uint32_t steps = 0u;

    while (!is_move_forward_complete()) {
        struct move_forward_errors errors = calculate_move_forward_errors(&state, cfg);
        struct motor_output output = calculate_move_forward_motor_output(errors, cfg.control);

        apply_motor_output(output);

        if (++steps > MAX_STEPS) {
            break;
        }
    }

    stop_motors();
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
static int32_t clamp_int32(int32_t val, int32_t min, int32_t max)
{
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    }
    return val;
}

void init_move_forward_state(struct move_forward_state *state)
{
    memset(state, 0, sizeof(*state));

    clear_1_encoder_ticks();
    clear_2_encoder_ticks();

    set_wheel_motor_1_direction_forward();
    set_wheel_motor_2_direction_forward();
}

bool is_move_forward_complete(void)
{
    int32_t avg_ticks = (abs(get_encoder_1_ticks()) + abs(get_encoder_2_ticks())) / 2;

    return avg_ticks >= navigation_params.move_forward_one_cell_target_ticks;
}

struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         struct move_forward_config cfg)
{
    struct move_forward_errors errors = {0};

    int32_t enc_1_ticks = get_encoder_1_ticks();
    int32_t enc_2_ticks = get_encoder_2_ticks();

    int32_t m1_velocity = enc_1_ticks - state->prev_enc_1_ticks;
    int32_t m2_velocity = enc_2_ticks - state->prev_enc_2_ticks;

    errors.velocity_error = m2_velocity - m1_velocity;
    errors.velocity_derivative = errors.velocity_error - state->prev_velocity_error;

    errors.angle_error = enc_2_ticks - enc_1_ticks;
    errors.angle_derivative = errors.angle_error - state->prev_angle_error;

    state->prev_enc_1_ticks = enc_1_ticks;
    state->prev_enc_2_ticks = enc_2_ticks;

    state->prev_velocity_error = errors.velocity_error;
    state->prev_angle_error = errors.angle_error;

    if (cfg.wall_mode != WALL_FEEDBACK_NONE) {

        int32_t ir_2 = (int32_t)read_ir_2_sensor();
        int32_t ir_3 = (int32_t)read_ir_3_sensor();

        int32_t target_ir = (int32_t)cfg.control.wall_target;

        if (cfg.wall_mode == WALL_FEEDBACK_LEFT) {
            errors.ir_error = target_ir - ir_2;
        } else if (cfg.wall_mode == WALL_FEEDBACK_RIGHT) {
            errors.ir_error = -(target_ir - ir_3);
        } else if (cfg.wall_mode == WALL_FEEDBACK_BOTH) {
            errors.ir_error = ir_3 - ir_2;
        }
    }
    errors.ir_derivative = errors.ir_error - state->prev_ir_error;
    state->prev_ir_error = errors.ir_error;

    return errors;
}

struct motor_output calculate_move_forward_motor_output(struct move_forward_errors errors,
                                                        struct move_forward_control_config cfg)
{
    struct motor_output output = {0};

    int64_t p_term_vel = (int64_t)cfg.kp_velocity * errors.velocity_error;
    int64_t d_term_vel = (int64_t)cfg.kd_velocity * errors.velocity_derivative;
    int64_t p_term_ang = (int64_t)cfg.kp_angle * errors.angle_error;
    int64_t d_term_ang = (int64_t)cfg.kd_angle * errors.angle_derivative;
    int64_t enc_control = p_term_vel + d_term_vel + p_term_ang + d_term_ang;

    int64_t p_term_ir = (int64_t)cfg.kp_ir * errors.ir_error;
    int64_t d_term_ir = (int64_t)cfg.kd_ir * errors.ir_derivative;
    int64_t ir_control = p_term_ir + d_term_ir;

    int64_t control_64 = enc_control + ir_control;
    int32_t control = (int32_t)(control_64 / cfg.pid_scale);

    int32_t speed_1 = clamp_int32(cfg.base_speed + control, cfg.min_speed, cfg.max_speed);
    int32_t speed_2 = clamp_int32(cfg.base_speed - control, cfg.min_speed, cfg.max_speed);

    output.motor_1_speed = (uint8_t)speed_1;
    output.motor_2_speed = (uint8_t)speed_2;

    return output;
}

void apply_motor_output(struct motor_output output)
{
    set_wheel_motor_1_speed(output.motor_1_speed);
    set_wheel_motor_2_speed(output.motor_2_speed);
}

void stop_motors(void)
{
    set_wheel_motor_1_speed(0u);
    set_wheel_motor_2_speed(0u);
}
