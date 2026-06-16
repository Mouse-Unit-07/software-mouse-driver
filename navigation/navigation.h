/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : navigation.h                                          */
/*                                                                            */
/* Interface for navigation library                                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef NAVIGATION_H_
#define NAVIGATION_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
struct mouse_physical_params {
    double wheel_diameter_mm;
    double wheel_base_mm;
    double max_motor_rpm;
    double encoder_events_per_revolution;
    double motor_pinion_gear_teeth;
    double wheel_gear_teeth;
};

struct mouse_calculated_params {
    double gear_ratio;
    double encoder_ticks_per_revolution;
    double encoder_ticks_per_millimeter;
    double encoder_ticks_per_rotation_radian;
};

struct maze_physical_params {
    double post_size_mm;
    double wall_size_mm;
};

struct maze_calculated_params {
    double cell_size_mm;
};

struct navigation_params {
    int32_t move_forward_one_cell_target_ticks;
    int32_t rotate_90_degree_target_ticks;
    int32_t rotate_180_degree_target_ticks;
};

/* helper structs exposed for testing */
struct motor_output {
    uint8_t motor_1_speed;
    uint8_t motor_2_speed;
};

/*----------------------------------------------------------------------------*/
/* move forward */
struct move_forward_control_config {
    uint8_t base_speed;
    uint8_t min_speed;
    uint8_t max_speed;
    int32_t kp_velocity;
    int32_t kd_velocity;
    int32_t kp_angle;
    int32_t kd_angle;
    int32_t kp_ir;
    int32_t kd_ir;
    int32_t pid_scale;
    uint32_t wall_target;
};

enum wall_feedback_mode
{
    WALL_FEEDBACK_NONE,
    WALL_FEEDBACK_LEFT,
    WALL_FEEDBACK_RIGHT,
    WALL_FEEDBACK_BOTH
};

/* helper structs exposed for testing */
struct move_forward_state {
    int32_t prev_enc_1_ticks;
    int32_t prev_enc_2_ticks;
    int32_t prev_velocity_error;
    int32_t prev_angle_error;
    int32_t prev_ir_error;
};

struct move_forward_errors {
    int32_t velocity_error;
    int32_t velocity_derivative;
    int32_t angle_error;
    int32_t angle_derivative;
    int32_t ir_error;
    int32_t ir_derivative;
};

/*----------------------------------------------------------------------------*/
/* rotation */
enum rotation_direction
{
    ROTATE_CLOCKWISE,
    ROTATE_COUNTER_CLOCKWISE
};

struct rotate_control_config {
    uint8_t base_speed;
    uint8_t min_speed;
    uint8_t max_speed;
    int32_t kp_velocity;
    int32_t kd_velocity;
    int32_t kp_angle;
    int32_t kd_angle;
    int32_t pid_scale;
};

/* helper structs exposed for testing */
struct rotate_state {
    int32_t prev_enc_1_ticks;
    int32_t prev_enc_2_ticks;
    int32_t prev_velocity_error;
    int32_t prev_angle_error;
};

struct rotate_errors {
    int32_t velocity_error;
    int32_t velocity_derivative;
    int32_t angle_error;
    int32_t angle_derivative;
};

/*----------------------------------------------------------------------------*/
/* side-wall detection */
struct side_wall_detection_config {
    uint32_t reading_threshold;
    uint32_t slope_threshold;
    uint32_t num_detection_samples;
    double reading_start_offset;
};

struct side_wall_calculated_params {
    uint32_t reading_start_offset_ticks;
};

/* helper structs exposed for testing */
struct side_wall_detector {
    bool have_previous_reading;
    uint32_t prev_left_reading;
    uint32_t prev_right_reading;
    bool left_first_change_recorded;
    bool right_first_change_recorded;
    bool left_wall_present_on_first_change;
    bool right_wall_present_on_first_change;
    bool left_wall_currently_present;
    bool right_wall_currently_present;

    uint64_t left_sum;
    uint64_t right_sum;
    uint32_t samples_collected;
};

/*----------------------------------------------------------------------------*/
/* front-wall detection */
struct front_wall_detection_config {
    uint32_t reading_threshold;
    uint32_t num_detection_samples;
};

/*----------------------------------------------------------------------------*/
/* telemetry */
struct move_forward_statistics {
    uint32_t control_loop_iterations;
    int32_t final_encoder_1_ticks;
    int32_t final_encoder_2_ticks;
    bool left_wall_present;
    bool right_wall_present;
    bool timeout_occurred;
};

struct rotate_statistics {
    uint32_t control_loop_iterations;
    int32_t final_encoder_1_ticks;
    int32_t final_encoder_2_ticks;
    bool timeout_occurred;
};

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_navigation(void);
void deinit_navigation(void);
void calculate_mouse_params(struct mouse_physical_params p);
void calculate_maze_params(struct maze_physical_params p);
void calculate_navigation_params(void);
struct mouse_physical_params get_mouse_physical_params(void);
struct mouse_calculated_params get_mouse_calculated_params(void);
struct maze_physical_params get_maze_physical_params(void);
struct maze_calculated_params get_maze_calculated_params(void);
struct navigation_params get_navigation_params(void);

/* helpers exposed for testing */
bool is_tick_average_at_target(int32_t target_ticks);
void apply_motor_output(struct motor_output output);
void stop_motors(void);

/*----------------------------------------------------------------------------*/
/* move forward */
void move_forward(void);

void set_no_wall_move_forward_control_config(struct move_forward_control_config cfg);
void set_one_wall_move_forward_control_config(struct move_forward_control_config cfg);
void set_both_wall_move_forward_control_config(struct move_forward_control_config cfg);
struct move_forward_control_config get_no_wall_move_forward_control_config(void);
struct move_forward_control_config get_one_wall_move_forward_control_config(void);
struct move_forward_control_config get_both_wall_move_forward_control_config(void);

/* helpers exposed for testing */
void init_move_forward_state(struct move_forward_state *state);
void move_forward_with_wall_mode(enum wall_feedback_mode initial_mode, bool avoid_mode_switching);
struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         enum wall_feedback_mode wall_mode,
                                                         uint32_t wall_target);
struct motor_output calculate_move_forward_motor_output(struct move_forward_errors errors,
                                                        struct move_forward_control_config cfg);
void reset_move_forward_error_history(struct move_forward_state *state);

/*----------------------------------------------------------------------------*/
/* rotation */
void rotate_clockwise_90_deg(void);
void rotate_counter_clockwise_90_deg(void);
void rotate_180_deg(void);

void set_rotate_control_config(struct rotate_control_config cfg);
struct rotate_control_config get_rotate_control_config(void);

/* helpers exposed for testing */
void init_rotate_state(struct rotate_state *state, enum rotation_direction direction);
void rotate(enum rotation_direction direction, int32_t target_ticks);
struct rotate_errors calculate_rotate_errors(struct rotate_state *state);
struct motor_output calculate_rotate_motor_output(struct rotate_errors errors,
                                                  struct rotate_control_config cfg);

/*----------------------------------------------------------------------------*/
/* side-wall detection */
bool is_left_wall_present(void);
bool is_right_wall_present(void);

void set_side_wall_detection_config(struct side_wall_detection_config cfg);
struct side_wall_detection_config get_side_wall_detection_config(void);
struct side_wall_calculated_params get_side_wall_calculated_params(void);

/* helpers exposed for testing */
void init_side_wall_detector(struct side_wall_detector *detector);
void update_side_wall_detector(struct side_wall_detector *detector);
enum wall_feedback_mode determine_wall_mode(const struct side_wall_detector *detector);
bool determine_wall_presence(const struct side_wall_detector *detector,
                             bool left_presence_requested);

/*----------------------------------------------------------------------------*/
/* front-wall detection */
bool is_front_wall_present(void);

void set_front_wall_detection_config(struct front_wall_detection_config cfg);
struct front_wall_detection_config get_front_wall_detection_config(void);

/*----------------------------------------------------------------------------*/
/* telemetry */
struct move_forward_statistics get_move_forward_statistics(void);
struct rotate_statistics get_rotate_statistics(void);

#endif /* NAVIGATION_H_ */
