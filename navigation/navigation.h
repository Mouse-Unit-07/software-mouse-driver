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

struct move_forward_config {
    enum wall_feedback_mode wall_mode;
    struct move_forward_control_config control;
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

struct motor_output {
    uint8_t motor_1_speed;
    uint8_t motor_2_speed;
};

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_navigation(void);
void deinit_navigation(void);
void calculate_mouse_params(struct mouse_physical_params p);
void calculate_maze_params(struct maze_physical_params p);
void calculate_navigation_params(void);

void move_forward(struct move_forward_config cfg);

/* helpers exposed for testing */
void init_move_forward_state(struct move_forward_state *state);
bool is_move_forward_complete(void);
struct move_forward_errors calculate_move_forward_errors(struct move_forward_state *state,
                                                         struct move_forward_config cfg);
struct motor_output calculate_move_forward_motor_output(struct move_forward_errors errors,
                                                        struct move_forward_control_config cfg);

void apply_motor_output(struct motor_output output);
void stop_motors(void);

#endif /* NAVIGATION_H_ */
