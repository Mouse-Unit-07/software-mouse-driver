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

struct maze_physical_params {
    double post_size_mm;
    double wall_size_mm;
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
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_navigation(void);
void deinit_navigation(void);
void calculate_mouse_params(struct mouse_physical_params p);
void calculate_maze_params(struct maze_physical_params p);
void calculate_navigation_params(void);

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
void move_forward_with_wall_mode(enum wall_feedback_mode mode);
struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         enum wall_feedback_mode wall_mode,
                                                         uint32_t wall_target);
struct motor_output calculate_move_forward_motor_output(struct move_forward_errors errors,
                                                        struct move_forward_control_config cfg);

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
                                                  enum rotation_direction direction,
                                                  struct rotate_control_config cfg);

#endif /* NAVIGATION_H_ */
