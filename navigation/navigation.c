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
/* none */

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static int32_t clamp_int32(int32_t val, int32_t min, int32_t max);
static int32_t round_to_int32(double value);
static uint32_t round_positive_to_uint32(double value);

static void reset_navigation_state(void);
static int32_t calculate_move_forward_drift_ticks(struct move_forward_control_config cfg);
static struct move_forward_calculated_params
get_move_forward_calculated_params(enum wall_feedback_mode mode);
static void calculate_side_wall_params(void);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static struct mouse_physical_params mouse_physical_params = {0};
static struct mouse_calculated_params mouse_params = {0};
static struct maze_physical_params maze_physical_params = {0};
static struct maze_calculated_params maze_params = {0};
static struct navigation_params navigation_params = {0};

static struct move_forward_common_config move_forward_common_config = {0};
static struct move_forward_control_config no_wall_move_forward_control_config = {0};
static struct move_forward_control_config one_wall_move_forward_control_config = {0};
static struct move_forward_control_config both_wall_move_forward_control_config = {0};
static struct move_forward_calculated_params no_wall_move_forward_calculated_params = {0};
static struct move_forward_calculated_params one_wall_move_forward_calculated_params = {0};
static struct move_forward_calculated_params both_wall_move_forward_calculated_params = {0};
static struct rotate_control_config rotate_control_config = {0};
static struct side_wall_detection_config side_wall_detection_config = {0};
static struct side_wall_calculated_params side_wall_calculated_params = {0};
static struct front_wall_detection_config front_wall_detection_config = {0};

static bool left_wall_present = false;
static bool right_wall_present = false;

static struct move_forward_statistics move_forward_statistics = {0};
static struct rotate_statistics rotate_statistics = {0};

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_navigation(void)
{
    reset_navigation_state();
}

void deinit_navigation(void)
{
    reset_navigation_state();
}

void calculate_mouse_params(struct mouse_physical_params p)
{
    mouse_physical_params = p;

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
    maze_physical_params = p;

    maze_params.cell_size_mm = p.post_size_mm + p.wall_size_mm;
}

void calculate_navigation_params(void)
{
    navigation_params.move_forward_one_cell_target_ticks = (int32_t)round_to_int32(
        maze_params.cell_size_mm * mouse_params.encoder_ticks_per_millimeter);

    navigation_params.rotate_90_degree_target_ticks =
        (int32_t)round_to_int32((M_PI / 2.0) * mouse_params.encoder_ticks_per_rotation_radian);

    navigation_params.rotate_180_degree_target_ticks =
        (int32_t)round_to_int32(M_PI * mouse_params.encoder_ticks_per_rotation_radian);

    calculate_side_wall_params();
}

struct mouse_physical_params get_mouse_physical_params(void)
{
    return mouse_physical_params;
}

struct mouse_calculated_params get_mouse_calculated_params(void)
{
    return mouse_params;
}

struct maze_physical_params get_maze_physical_params(void)
{
    return maze_physical_params;
}

struct maze_calculated_params get_maze_calculated_params(void)
{
    return maze_params;
}

struct navigation_params get_navigation_params(void)
{
    return navigation_params;
}

/*----------------------------------------------------------------------------*/
/* move forward */
static int32_t calculate_move_forward_drift_ticks(struct move_forward_control_config cfg)
{
    return round_to_int32((0.24 * cfg.base_speed) - 6.0);
}

void set_move_forward_common_config(struct move_forward_common_config cfg)
{
    move_forward_common_config = cfg;
}

struct move_forward_common_config get_move_forward_common_config(void)
{
    return move_forward_common_config;
}

void set_no_wall_move_forward_control_config(struct move_forward_control_config cfg)
{
    no_wall_move_forward_control_config = cfg;

    no_wall_move_forward_calculated_params.drift_ticks = calculate_move_forward_drift_ticks(cfg);
}

void set_one_wall_move_forward_control_config(struct move_forward_control_config cfg)
{
    one_wall_move_forward_control_config = cfg;

    one_wall_move_forward_calculated_params.drift_ticks = calculate_move_forward_drift_ticks(cfg);
}

void set_both_wall_move_forward_control_config(struct move_forward_control_config cfg)
{
    both_wall_move_forward_control_config = cfg;

    both_wall_move_forward_calculated_params.drift_ticks = calculate_move_forward_drift_ticks(cfg);
}

struct move_forward_control_config get_no_wall_move_forward_control_config(void)
{
    return no_wall_move_forward_control_config;
}

struct move_forward_control_config get_one_wall_move_forward_control_config(void)
{
    return one_wall_move_forward_control_config;
}

struct move_forward_control_config get_both_wall_move_forward_control_config(void)
{
    return both_wall_move_forward_control_config;
}

struct move_forward_calculated_params get_no_wall_move_forward_calculated_params(void)
{
    return no_wall_move_forward_calculated_params;
}

struct move_forward_calculated_params get_one_wall_move_forward_calculated_params(void)
{
    return one_wall_move_forward_calculated_params;
}

struct move_forward_calculated_params get_both_wall_move_forward_calculated_params(void)
{
    return both_wall_move_forward_calculated_params;
}

void move_forward(void)
{
    move_forward_with_wall_mode(WALL_FEEDBACK_NONE, false);
}

uint32_t move_forward_until_turn_or_intersection_and_return_steps(void)
{
    uint32_t steps = 0u;

    do {
        move_forward_with_wall_mode(WALL_FEEDBACK_NONE, false);
        steps++;
    } while (should_continue_straight());

    return steps;
}

/*----------------------------------------------------------------------------*/
/* rotation */
void set_rotate_control_config(struct rotate_control_config cfg)
{
    rotate_control_config = cfg;
}

struct rotate_control_config get_rotate_control_config(void)
{
    return rotate_control_config;
}

void rotate_clockwise_90_deg(void)
{
    rotate(ROTATE_CLOCKWISE, navigation_params.rotate_90_degree_target_ticks);
}

void rotate_counter_clockwise_90_deg(void)
{
    rotate(ROTATE_COUNTER_CLOCKWISE, navigation_params.rotate_90_degree_target_ticks);
}

void rotate_180_deg(void)
{
    rotate(ROTATE_CLOCKWISE, navigation_params.rotate_180_degree_target_ticks);
}

/*----------------------------------------------------------------------------*/
/* side-wall detection */
bool is_left_wall_present(void)
{
    return left_wall_present;
}

bool is_right_wall_present(void)
{
    return right_wall_present;
}

void set_side_wall_detection_config(struct side_wall_detection_config cfg)
{
    side_wall_detection_config = cfg;

    calculate_side_wall_params();
}

struct side_wall_detection_config get_side_wall_detection_config(void)
{
    return side_wall_detection_config;
}

struct side_wall_calculated_params get_side_wall_calculated_params(void)
{
    return side_wall_calculated_params;
}

/*----------------------------------------------------------------------------*/
/* front-wall detection */
bool is_front_wall_present(void)
{
    uint64_t sum = 0u;

    for (uint32_t i = 0u; i < front_wall_detection_config.num_detection_samples; i++) {
        sum += read_ir_1_sensor();
        sum += read_ir_4_sensor();
    }

    if (front_wall_detection_config.num_detection_samples == 0u) {
        return false;
    }

    uint32_t average = (uint32_t)(sum / (front_wall_detection_config.num_detection_samples * 2u));

    return average >= front_wall_detection_config.reading_threshold;
}

void set_front_wall_detection_config(struct front_wall_detection_config cfg)
{
    front_wall_detection_config = cfg;
}

struct front_wall_detection_config get_front_wall_detection_config(void)
{
    return front_wall_detection_config;
}

/*----------------------------------------------------------------------------*/
/* telemetry */
struct move_forward_statistics get_move_forward_statistics(void)
{
    return move_forward_statistics;
}

struct rotate_statistics get_rotate_statistics(void)
{
    return rotate_statistics;
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

static int32_t round_to_int32(double value)
{
    return (value >= 0.0) ? (int32_t)(value + 0.5) : (int32_t)(value - 0.5);
}

static uint32_t round_positive_to_uint32(double value)
{
    return (uint32_t)(value + 0.5);
}

static void reset_navigation_state(void)
{
    memset(&mouse_physical_params, 0, sizeof(mouse_physical_params));
    memset(&mouse_params, 0, sizeof(mouse_params));
    memset(&maze_physical_params, 0, sizeof(maze_physical_params));
    memset(&maze_params, 0, sizeof(maze_params));
    memset(&navigation_params, 0, sizeof(navigation_params));

    memset(&move_forward_common_config, 0, sizeof(move_forward_common_config));
    memset(&no_wall_move_forward_control_config, 0, sizeof(no_wall_move_forward_control_config));
    memset(&one_wall_move_forward_control_config, 0, sizeof(one_wall_move_forward_control_config));
    memset(&both_wall_move_forward_control_config, 0,
           sizeof(both_wall_move_forward_control_config));
    memset(&no_wall_move_forward_calculated_params, 0,
           sizeof(no_wall_move_forward_calculated_params));
    memset(&one_wall_move_forward_calculated_params, 0,
           sizeof(one_wall_move_forward_calculated_params));
    memset(&both_wall_move_forward_calculated_params, 0,
           sizeof(both_wall_move_forward_calculated_params));
    memset(&rotate_control_config, 0, sizeof(rotate_control_config));

    memset(&side_wall_detection_config, 0, sizeof(side_wall_detection_config));
    memset(&side_wall_calculated_params, 0, sizeof(side_wall_calculated_params));

    memset(&front_wall_detection_config, 0, sizeof(front_wall_detection_config));

    left_wall_present = false;
    right_wall_present = false;

    memset(&move_forward_statistics, 0, sizeof(move_forward_statistics));
    memset(&rotate_statistics, 0, sizeof(rotate_statistics));
}

bool is_tick_average_at_target(int32_t target_ticks)
{
    int32_t avg_ticks = (abs(get_encoder_1_ticks()) + abs(get_encoder_2_ticks())) / 2;

    return avg_ticks >= target_ticks;
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

/*----------------------------------------------------------------------------*/
/* move forward */
void init_move_forward_state(struct move_forward_state *state)
{
    memset(state, 0, sizeof(*state));

    clear_1_encoder_ticks();
    clear_2_encoder_ticks();

    set_wheel_motor_1_direction_forward();
    set_wheel_motor_2_direction_forward();
}

static struct move_forward_calculated_params
get_move_forward_calculated_params(enum wall_feedback_mode mode)
{
    if (mode == WALL_FEEDBACK_NONE) {
        return no_wall_move_forward_calculated_params;
    } else if (mode == WALL_FEEDBACK_BOTH) {
        return both_wall_move_forward_calculated_params;
    } else {
        return one_wall_move_forward_calculated_params;
    }

    struct move_forward_calculated_params params = {0};
    return params;
}

bool emergency_stop_detected(void)
{
    uint32_t threshold = move_forward_common_config.emergency_stop_threshold;

    if (threshold == 0u) {
        return false;
    }

    uint32_t average = (read_ir_1_sensor() + read_ir_4_sensor()) / 2u;

    return average >= threshold;
}

bool should_continue_straight(void)
{
    bool left_wall = is_left_wall_present();
    bool front_wall = is_front_wall_present();
    bool right_wall = is_right_wall_present();

    if (front_wall) {
        return false;
    }

    if (!left_wall) {
        return false;
    }

    if (!right_wall) {
        return false;
    }

    return true;
}

void move_forward_with_wall_mode(enum wall_feedback_mode initial_mode, bool avoid_mode_switching)
{
    struct move_forward_state state = {0};
    struct side_wall_detector detector = {0};
    struct side_wall_readings readings = {0};

    init_move_forward_state(&state);
    init_side_wall_detector(&detector);

    memset(&move_forward_statistics, 0, sizeof(move_forward_statistics));

    const uint32_t MAX_STEPS = 10000u;
    uint32_t steps = 0u;

    enum wall_feedback_mode previous_mode = initial_mode;

    struct move_forward_calculated_params calculated_params =
        get_move_forward_calculated_params(initial_mode);

    while (!is_tick_average_at_target(navigation_params.move_forward_one_cell_target_ticks
                                      - calculated_params.drift_ticks)) {
        if (emergency_stop_detected()) {
            move_forward_statistics.emergency_stop_occurred = true;
            break;
        }

        update_side_wall_detector(&detector, &readings);

        enum wall_feedback_mode mode = initial_mode;
        if (!avoid_mode_switching) {
            mode = determine_wall_mode(&detector);

            if (mode != previous_mode) {
                reset_move_forward_error_history(&state);
                calculated_params = get_move_forward_calculated_params(mode);
                previous_mode = mode;
            }
        }
        struct move_forward_control_config cfg = {0};
        if (mode == WALL_FEEDBACK_NONE) {
            cfg = no_wall_move_forward_control_config;
        } else if (mode == WALL_FEEDBACK_BOTH) {
            cfg = both_wall_move_forward_control_config;
        } else {
            cfg = one_wall_move_forward_control_config;
        }

        struct move_forward_errors errors =
            calculate_move_forward_errors(&state, mode, cfg.wall_target, &readings);
        struct motor_output output = calculate_move_forward_motor_output(errors, cfg);

        apply_motor_output(output);

        if (++steps > MAX_STEPS) {
            move_forward_statistics.timeout_occurred = true;
            break;
        }
    }

    stop_motors();

    left_wall_present = determine_wall_presence(&detector, true);
    right_wall_present = determine_wall_presence(&detector, false);

    move_forward_statistics.control_loop_iterations = steps;
    move_forward_statistics.final_encoder_1_ticks = get_encoder_1_ticks();
    move_forward_statistics.final_encoder_2_ticks = get_encoder_2_ticks();
    move_forward_statistics.left_wall_present = left_wall_present;
    move_forward_statistics.right_wall_present = right_wall_present;
}

struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         enum wall_feedback_mode wall_mode,
                                                         uint32_t wall_target,
                                                         struct side_wall_readings const *readings)
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

    if (wall_mode != WALL_FEEDBACK_NONE) {
        int32_t ir_2 = (int32_t)readings->left;
        int32_t ir_3 = (int32_t)readings->right;

        int32_t target_ir = (int32_t)wall_target;

        if (wall_mode == WALL_FEEDBACK_LEFT) {
            errors.ir_error = -(target_ir - ir_2);
        } else if (wall_mode == WALL_FEEDBACK_RIGHT) {
            errors.ir_error = target_ir - ir_3;
        } else if (wall_mode == WALL_FEEDBACK_BOTH) {
            errors.ir_error = ir_2 - ir_3;
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

void reset_move_forward_error_history(struct move_forward_state *state)
{
    state->prev_velocity_error = 0;
    state->prev_angle_error = 0;
    state->prev_ir_error = 0;

    state->prev_enc_1_ticks = get_encoder_1_ticks();
    state->prev_enc_2_ticks = get_encoder_2_ticks();
}

/*----------------------------------------------------------------------------*/
/* rotation */
void init_rotate_state(struct rotate_state *state, enum rotation_direction direction)
{
    memset(state, 0, sizeof(*state));

    clear_1_encoder_ticks();
    clear_2_encoder_ticks();

    if (direction == ROTATE_CLOCKWISE) {
        set_wheel_motor_1_direction_forward();
        set_wheel_motor_2_direction_backward();
    } else {
        set_wheel_motor_1_direction_backward();
        set_wheel_motor_2_direction_forward();
    }
}

void rotate(enum rotation_direction direction, int32_t target_ticks)
{
    struct rotate_state state = {0};

    init_rotate_state(&state, direction);

    memset(&rotate_statistics, 0, sizeof(rotate_statistics));

    const uint32_t MAX_STEPS = 30000u;
    uint32_t steps = 0u;

    while (!is_tick_average_at_target(target_ticks)) {
        struct rotate_errors errors = calculate_rotate_errors(&state);
        struct motor_output output = calculate_rotate_motor_output(errors, rotate_control_config);

        apply_motor_output(output);

        if (++steps > MAX_STEPS) {
            rotate_statistics.timeout_occurred = true;
            break;
        }
    }

    stop_motors();

    rotate_statistics.control_loop_iterations = steps;
    rotate_statistics.final_encoder_1_ticks = get_encoder_1_ticks();
    rotate_statistics.final_encoder_2_ticks = get_encoder_2_ticks();
}

struct rotate_errors calculate_rotate_errors(struct rotate_state *state)
{
    struct rotate_errors errors = {0};

    int32_t enc_1_ticks = abs(get_encoder_1_ticks());
    int32_t enc_2_ticks = abs(get_encoder_2_ticks());

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

    return errors;
}

struct motor_output calculate_rotate_motor_output(struct rotate_errors errors,
                                                  struct rotate_control_config cfg)
{
    struct motor_output output = {0};

    int64_t p_term_vel = (int64_t)cfg.kp_velocity * errors.velocity_error;
    int64_t d_term_vel = (int64_t)cfg.kd_velocity * errors.velocity_derivative;
    int64_t p_term_ang = (int64_t)cfg.kp_angle * errors.angle_error;
    int64_t d_term_ang = (int64_t)cfg.kd_angle * errors.angle_derivative;

    int64_t control_64 = p_term_vel + d_term_vel + p_term_ang + d_term_ang;
    int32_t control = (int32_t)(control_64 / cfg.pid_scale);

    int32_t speed_1 = 0;
    int32_t speed_2 = 0;

    speed_1 = cfg.base_speed + control;
    speed_2 = cfg.base_speed - control;

    speed_1 = clamp_int32(speed_1, cfg.min_speed, cfg.max_speed);
    speed_2 = clamp_int32(speed_2, cfg.min_speed, cfg.max_speed);

    output.motor_1_speed = (uint8_t)speed_1;
    output.motor_2_speed = (uint8_t)speed_2;

    return output;
}

/*----------------------------------------------------------------------------*/
/* side-wall detection */
static void calculate_side_wall_params(void)
{
    side_wall_calculated_params.reading_start_offset_ticks =
        (uint32_t)round_positive_to_uint32(side_wall_detection_config.reading_start_offset
                                           * navigation_params.move_forward_one_cell_target_ticks);
}

void init_side_wall_detector(struct side_wall_detector *detector)
{
    memset(detector, 0, sizeof(*detector));
}

void update_side_wall_detector(struct side_wall_detector *detector,
                               struct side_wall_readings *readings)
{
    uint32_t current_step =
        (uint32_t)((abs(get_encoder_1_ticks()) + abs(get_encoder_2_ticks())) / 2);
    uint32_t start_step = side_wall_calculated_params.reading_start_offset_ticks;
    uint32_t left_reading = read_ir_2_sensor();
    uint32_t right_reading = read_ir_3_sensor();

    readings->left = left_reading;
    readings->right = right_reading;

    if ((current_step >= start_step)
        && (detector->samples_collected < side_wall_detection_config.num_detection_samples)) {
        detector->left_sum += left_reading;
        detector->right_sum += right_reading;
        detector->samples_collected++;
    }

    /* TODO: make this end step configurable w/ commands */
    uint32_t end_slope_detection_step =
        (navigation_params.move_forward_one_cell_target_ticks * 8) / 10;

    if (current_step < end_slope_detection_step) {
        if (detector->have_previous_reading) {
            if (left_reading > detector->prev_left_reading) {
                if ((left_reading - detector->prev_left_reading)
                    >= side_wall_detection_config.slope_threshold) {
                    detector->left_first_change_recorded = true;
                    detector->left_wall_currently_present = true;
                }
            } else {
                if ((detector->prev_left_reading - left_reading)
                    >= side_wall_detection_config.slope_threshold) {
                    detector->left_first_change_recorded = true;
                    detector->left_wall_currently_present = false;
                }
            }
            if (right_reading > detector->prev_right_reading) {
                if ((right_reading - detector->prev_right_reading)
                    >= side_wall_detection_config.slope_threshold) {
                    detector->right_first_change_recorded = true;
                    detector->right_wall_currently_present = true;
                }
            } else {
                if ((detector->prev_right_reading - right_reading)
                    >= side_wall_detection_config.slope_threshold) {
                    detector->right_first_change_recorded = true;
                    detector->right_wall_currently_present = false;
                }
            }
        }
    }

    detector->have_previous_reading = true;
    detector->prev_left_reading = left_reading;
    detector->prev_right_reading = right_reading;
}

enum wall_feedback_mode determine_wall_mode(const struct side_wall_detector *detector)
{
    bool left_wall_currently_present = false;
    bool right_wall_currently_present = false;

    if (detector->left_first_change_recorded) {
        left_wall_currently_present = detector->left_wall_currently_present;
    } else if (detector->samples_collected == 0u) {
        left_wall_currently_present = false;
    } else {
        left_wall_currently_present = (detector->left_sum / detector->samples_collected)
                                      >= side_wall_detection_config.reading_threshold;
    }

    if (detector->right_first_change_recorded) {
        right_wall_currently_present = detector->right_wall_currently_present;
    } else if (detector->samples_collected == 0u) {
        right_wall_currently_present = false;
    } else {
        right_wall_currently_present = (detector->right_sum / detector->samples_collected)
                                       >= side_wall_detection_config.reading_threshold;
    }

    if (left_wall_currently_present && right_wall_currently_present) {
        return WALL_FEEDBACK_BOTH;
    } else if (left_wall_currently_present && !right_wall_currently_present) {
        return WALL_FEEDBACK_LEFT;
    } else if (!left_wall_currently_present && right_wall_currently_present) {
        return WALL_FEEDBACK_RIGHT;
    }

    return WALL_FEEDBACK_NONE;
}

bool determine_wall_presence(const struct side_wall_detector *detector,
                             bool left_presence_requested)
{
    bool first_change_recorded;
    bool wall_present;
    uint64_t sum;

    if (left_presence_requested) {
        first_change_recorded = detector->left_first_change_recorded;
        wall_present = detector->left_wall_currently_present;
        sum = detector->left_sum;
    } else {
        first_change_recorded = detector->right_first_change_recorded;
        wall_present = detector->right_wall_currently_present;
        sum = detector->right_sum;
    }

    if (first_change_recorded) {
        return wall_present;
    }

    if (detector->samples_collected == 0) {
        return false;
    }

    return (sum / detector->samples_collected) >= side_wall_detection_config.reading_threshold;
}
