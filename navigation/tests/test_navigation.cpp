/*================================ FILE INFO =================================*/
/* Filename           : test_navigation.cpp                                   */
/*                                                                            */
/* Test implementation for navigation.c                                       */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdbool.h>
#include <stdint.h>
#include "navigation.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

/* ---------------------------------------------------------------------------*/
/* infrared sensor mocks and fakes */
uint32_t fake_ir_2_reading_value{0u};
uint32_t fake_ir_3_reading_value{0u};

uint32_t read_ir_2_sensor(void)
{
    return fake_ir_2_reading_value;
}

uint32_t read_ir_3_sensor(void)
{
    return fake_ir_3_reading_value;
}

void reset_ir_mocks(void)
{
    fake_ir_2_reading_value = 0u;
    fake_ir_3_reading_value = 0u;
}

/* ---------------------------------------------------------------------------*/
/* wheel motor mocks and fakes */
void set_wheel_motor_1_direction_forward(void)
{
    mock().actualCall("set_wheel_motor_1_direction_forward");
}

void set_wheel_motor_1_direction_backward(void)
{
    mock().actualCall("set_wheel_motor_1_direction_backward");
}

void set_wheel_motor_2_direction_forward(void)
{
    mock().actualCall("set_wheel_motor_2_direction_forward");
}

void set_wheel_motor_2_direction_backward(void)
{
    mock().actualCall("set_wheel_motor_2_direction_backward");
}

void set_wheel_motor_1_speed(uint8_t speed)
{
    mock().actualCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", speed);
}

void set_wheel_motor_2_speed(uint8_t speed)
{
    mock().actualCall("set_wheel_motor_2_speed")
        .withUnsignedIntParameter("speed", speed);
}

/* ---------------------------------------------------------------------------*/
/* encoder fakes */
int32_t fake_encoder_1_ticks{0};
int32_t fake_encoder_2_ticks{0};

int32_t get_encoder_1_ticks(void)
{
    return fake_encoder_1_ticks;
}

int32_t get_encoder_2_ticks(void)
{
    return fake_encoder_2_ticks;
}

void clear_1_encoder_ticks(void)
{
    mock().actualCall("clear_1_encoder_ticks");
    fake_encoder_1_ticks = 0;
}

void clear_2_encoder_ticks(void)
{
    mock().actualCall("clear_2_encoder_ticks");
    fake_encoder_2_ticks = 0;
}

void reset_encoder_mocks(void)
{
    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;
}

}

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
struct move_forward_control_config create_default_control_config(void)
{
    struct move_forward_control_config cfg = {0};

    cfg.base_speed = 200u;
    cfg.min_speed = 100u;
    cfg.max_speed = 255u;
    cfg.kp_velocity = 1;
    cfg.kd_velocity = 1;
    cfg.kp_angle = 1;
    cfg.kd_angle = 1;
    cfg.kp_ir = 1;
    cfg.kd_ir = 1;
    cfg.pid_scale = 1;
    cfg.wall_target = 500u;

    return cfg;
}

void set_move_forward_control_configs(void)
{
    set_no_wall_move_forward_control_config(create_default_control_config());
    set_one_wall_move_forward_control_config(create_default_control_config());
    set_both_wall_move_forward_control_config(create_default_control_config());
}

struct rotate_control_config create_default_rotate_control_config(void)
{
    struct rotate_control_config cfg = {0};

    cfg.base_speed = 200u;
    cfg.min_speed = 100u;
    cfg.max_speed = 255u;
    cfg.kp_velocity = 1;
    cfg.kd_velocity = 1;
    cfg.kp_angle = 1;
    cfg.kd_angle = 1;
    cfg.pid_scale = 1;

    return cfg;
}

void set_rotation_control_config(void)
{
    set_rotate_control_config(create_default_rotate_control_config());
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(NavigationTests)
{
    void setup() override
    {
        mock().clear();
        reset_encoder_mocks();
        reset_ir_mocks();
        set_move_forward_control_configs();
        set_rotation_control_config();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/*============================================================================*/
/*                                    Tests                                   */
/*============================================================================*/
TEST(NavigationTests, ApplyMotorOutputSetsMotorSpeeds)
{
    struct motor_output output{0};

    output.motor_1_speed = 123u;
    output.motor_2_speed = 234u;

    mock().expectOneCall("set_wheel_motor_1_speed")
        .withUnsignedIntParameter("speed", 123u);

    mock().expectOneCall("set_wheel_motor_2_speed")
        .withUnsignedIntParameter("speed", 234u);

    apply_motor_output(output);
}

TEST(NavigationTests, StopMotorsSetsMotorSpeedsToZero)
{
    mock().expectOneCall("set_wheel_motor_1_speed")
          .withUnsignedIntParameter("speed", 0u);

    mock().expectOneCall("set_wheel_motor_2_speed")
          .withUnsignedIntParameter("speed", 0u);

    stop_motors();
}

TEST(NavigationTests, IsTickAverageAtTargetReturnsFalseWhenBelowTarget)
{
    fake_encoder_1_ticks = 40;
    fake_encoder_2_ticks = 60;

    CHECK_FALSE(is_tick_average_at_target(60));
}

TEST(NavigationTests, IsTickAverageAtTargetReturnsTrueWhenAtTarget)
{
    fake_encoder_1_ticks = 50;
    fake_encoder_2_ticks = 70;

    CHECK(is_tick_average_at_target(60));
}

TEST(NavigationTests, IsTickAverageAtTargetUsesAbsoluteEncoderValues)
{
    fake_encoder_1_ticks = -80;
    fake_encoder_2_ticks = -120;

    CHECK(is_tick_average_at_target(100));
}

/*----------------------------------------------------------------------------*/
/* move forward */
TEST(NavigationTests, InitMoveForwardStateClearsStateAndInitializesHardware)
{
    struct move_forward_state state{0};

    state.prev_enc_1_ticks = 1;
    state.prev_enc_2_ticks = 2;
    state.prev_velocity_error = 3;
    state.prev_angle_error = 4;
    state.prev_ir_error = 5;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");
    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    init_move_forward_state(&state);

    CHECK(state.prev_enc_1_ticks == 0);
    CHECK(state.prev_enc_2_ticks == 0);
    CHECK(state.prev_velocity_error == 0);
    CHECK(state.prev_angle_error == 0);
    CHECK(state.prev_ir_error == 0);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesVelocityError)
{
    struct move_forward_state state{0};

    fake_encoder_1_ticks = 100;
    fake_encoder_2_ticks = 120;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u)};

    CHECK(errors.velocity_error == 20);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesVelocityDerivative)
{
    struct move_forward_state state{0};
    state.prev_velocity_error = 10;

    fake_encoder_1_ticks = 100;
    fake_encoder_2_ticks = 130;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u)};

    CHECK(errors.velocity_error == 30);
    CHECK(errors.velocity_derivative == 20);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesAngleError)
{
    struct move_forward_state state{0};

    fake_encoder_1_ticks = 50;
    fake_encoder_2_ticks = 80;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u)};

    CHECK(errors.angle_error == 30);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesAngleDerivative)
{
    struct move_forward_state state{0};
    state.prev_angle_error = 5;

    fake_encoder_1_ticks = 10;
    fake_encoder_2_ticks = 40;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u)};

    CHECK(errors.angle_error == 30);
    CHECK(errors.angle_derivative == 25);
}

TEST(NavigationTests, CalculateMoveForwardErrorsUpdatesState)
{
    struct move_forward_state state{0};

    fake_encoder_1_ticks = 70;
    fake_encoder_2_ticks = 90;

    calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u);

    CHECK(state.prev_enc_1_ticks == 70);
    CHECK(state.prev_enc_2_ticks == 90);

    CHECK(state.prev_velocity_error == 20);
    CHECK(state.prev_angle_error == 20);
}

TEST(NavigationTests, CalculateMoveForwardErrorsLeftWallModeCalculatesIrError)
{
    struct move_forward_state state{0};

    fake_ir_2_reading_value = 450;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_LEFT, 500u)};

    CHECK(errors.ir_error == 50);
}

TEST(NavigationTests, CalculateMoveForwardErrorsRightWallModeCalculatesIrError)
{
    struct move_forward_state state{0};

    fake_ir_3_reading_value = 450;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_RIGHT, 500u)};

    CHECK(errors.ir_error == -50);
}

TEST(NavigationTests, CalculateMoveForwardErrorsBothWallModeCalculatesIrError)
{
    struct move_forward_state state{0};

    fake_ir_2_reading_value = 400;
    fake_ir_3_reading_value = 550;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_BOTH, 0u)};

    CHECK(errors.ir_error == 150);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesIrDerivative)
{
    struct move_forward_state state{0};
    state.prev_ir_error = 10;

    fake_ir_2_reading_value = 450;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_LEFT, 500u)};

    CHECK(errors.ir_error == 50);
    CHECK(errors.ir_derivative == 40);
}

TEST(NavigationTests, CalculateMoveForwardErrorsNoWallModeLeavesIrErrorZero)
{
    struct move_forward_state state{0};

    fake_ir_2_reading_value = 1000;
    fake_ir_3_reading_value = 1000;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_NONE, 0u)};

    CHECK(errors.ir_error == 0);
    CHECK(errors.ir_derivative == 0);
}

TEST(NavigationTests, CalculateMoveForwardMotorOutputZeroErrorUsesBaseSpeed)
{
    struct move_forward_errors errors{0};
    struct move_forward_control_config cfg{create_default_control_config()};
    struct motor_output output{calculate_move_forward_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 200);
    CHECK(output.motor_2_speed == 200);
}

TEST(NavigationTests, CalculateMoveForwardMotorOutputPositiveControl)
{
    struct move_forward_errors errors{0};
    errors.velocity_error = 10;

    struct move_forward_control_config cfg{create_default_control_config()};
    struct motor_output output{calculate_move_forward_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed > output.motor_2_speed);
}

TEST(NavigationTests, CalculateMoveForwardMotorOutputNegativeControl)
{
    struct move_forward_errors errors{0};
    errors.velocity_error = -10;

    struct move_forward_control_config cfg{create_default_control_config()};
    struct motor_output output{calculate_move_forward_motor_output(errors, cfg)};

    CHECK(output.motor_2_speed > output.motor_1_speed);
}

TEST(NavigationTests, CalculateMoveForwardMotorOutputClampsMaximum)
{
    struct move_forward_errors errors{0};
    errors.velocity_error = 100000;

    struct move_forward_control_config cfg{create_default_control_config()};
    struct motor_output output{calculate_move_forward_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 255);
}

TEST(NavigationTests, CalculateMoveForwardMotorOutputClampsMinimum)
{
    struct move_forward_errors errors{0};

    errors.velocity_error = -100000;

    struct move_forward_control_config cfg{create_default_control_config()};
    struct motor_output output{calculate_move_forward_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 100);
}

TEST(NavigationTests, MoveForwardStopsMotorsWhenMaxStepsExceeded)
{
    struct mouse_physical_params mouse_params{};
    mouse_params.wheel_diameter_mm = 32.0;
    mouse_params.wheel_base_mm = 90.0;
    mouse_params.encoder_events_per_revolution = 60.0;
    mouse_params.motor_pinion_gear_teeth = 13.0;
    mouse_params.wheel_gear_teeth = 44.0;

    struct maze_physical_params maze_params{};
    maze_params.post_size_mm = 168.0;
    maze_params.wall_size_mm = 12.0;

    calculate_mouse_params(mouse_params);
    calculate_maze_params(maze_params);
    calculate_navigation_params();

    /* encoder ticks never change */
    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    mock().ignoreOtherCalls();

    move_forward();
}

/*----------------------------------------------------------------------------*/
/* rotation */
TEST(NavigationTests, InitRotateStateClockwiseInitializesHardware)
{
    struct rotate_state state{0};

    state.prev_enc_1_ticks = 1;
    state.prev_enc_2_ticks = 2;
    state.prev_velocity_error = 3;
    state.prev_angle_error = 4;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    init_rotate_state(&state, ROTATE_CLOCKWISE);

    CHECK(state.prev_enc_1_ticks == 0);
    CHECK(state.prev_enc_2_ticks == 0);
    CHECK(state.prev_velocity_error == 0);
    CHECK(state.prev_angle_error == 0);
}

TEST(NavigationTests, InitRotateStateCounterClockwiseInitializesHardware)
{
    struct rotate_state state{0};

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

    init_rotate_state(&state, ROTATE_COUNTER_CLOCKWISE);
}

TEST(NavigationTests, CalculateRotateErrorsCalculatesVelocityError)
{
    struct rotate_state state{0};

    fake_encoder_1_ticks = 100;
    fake_encoder_2_ticks = 130;

    struct rotate_errors errors{calculate_rotate_errors(&state)};

    CHECK(errors.velocity_error == 30);
}

TEST(NavigationTests, CalculateRotateErrorsCalculatesVelocityDerivative)
{
    struct rotate_state state{0};
    state.prev_velocity_error = 10;

    fake_encoder_1_ticks = 100;
    fake_encoder_2_ticks = 140;

    struct rotate_errors errors{calculate_rotate_errors(&state)};

    CHECK(errors.velocity_error == 40);
    CHECK(errors.velocity_derivative == 30);
}

TEST(NavigationTests, CalculateRotateErrorsCalculatesAngleError)
{
    struct rotate_state state{0};

    fake_encoder_1_ticks = 50;
    fake_encoder_2_ticks = 90;

    struct rotate_errors errors{calculate_rotate_errors(&state)};

    CHECK(errors.angle_error == 40);
}

TEST(NavigationTests, CalculateRotateErrorsCalculatesAngleDerivative)
{
    struct rotate_state state{0};
    state.prev_angle_error = 5;

    fake_encoder_1_ticks = 20;
    fake_encoder_2_ticks = 50;

    struct rotate_errors errors{calculate_rotate_errors(&state)};

    CHECK(errors.angle_error == 30);
    CHECK(errors.angle_derivative == 25);
}

TEST(NavigationTests, CalculateRotateErrorsUpdatesState)
{
    struct rotate_state state{0};

    fake_encoder_1_ticks = 70;
    fake_encoder_2_ticks = 100;

    calculate_rotate_errors(&state);

    CHECK(state.prev_enc_1_ticks == 70);
    CHECK(state.prev_enc_2_ticks == 100);

    CHECK(state.prev_velocity_error == 30);
    CHECK(state.prev_angle_error == 30);
}

TEST(NavigationTests, CalculateRotateErrorsUsesAbsoluteEncoderValues)
{
    struct rotate_state state{0};

    fake_encoder_1_ticks = -100;
    fake_encoder_2_ticks = -130;

    struct rotate_errors errors{calculate_rotate_errors(&state)};

    CHECK(errors.velocity_error == 30);
    CHECK(errors.angle_error == 30);
}

TEST(NavigationTests, CalculateRotateMotorOutputClockwiseZeroErrorUsesBaseSpeed)
{
    struct rotate_errors errors{0};
    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, ROTATE_CLOCKWISE, cfg)};

    CHECK(output.motor_1_speed == 200);
    CHECK(output.motor_2_speed == 200);
}

TEST(NavigationTests, CalculateRotateMotorOutputClockwisePositiveControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = 10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, ROTATE_CLOCKWISE, cfg)};

    CHECK(output.motor_1_speed > output.motor_2_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputClockwiseNegativeControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = -10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, ROTATE_CLOCKWISE, cfg)};

    CHECK(output.motor_2_speed > output.motor_1_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputCounterClockwisePositiveControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = 10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{
        calculate_rotate_motor_output(errors, ROTATE_COUNTER_CLOCKWISE, cfg)};

    CHECK(output.motor_2_speed > output.motor_1_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputCounterClockwiseNegativeControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = -10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{
        calculate_rotate_motor_output(errors, ROTATE_COUNTER_CLOCKWISE, cfg)};

    CHECK(output.motor_1_speed > output.motor_2_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputClampsMaximum)
{
    struct rotate_errors errors{0};
    errors.velocity_error = 100000;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, ROTATE_CLOCKWISE, cfg)};

    CHECK(output.motor_1_speed == 255);
}

TEST(NavigationTests, CalculateRotateMotorOutputClampsMinimum)
{
    struct rotate_errors errors{0};
    errors.velocity_error = -100000;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, ROTATE_CLOCKWISE, cfg)};

    CHECK(output.motor_1_speed == 100);
}

TEST(NavigationTests, RotateStopsMotorsWhenMaxStepsExceeded)
{
    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    mock().ignoreOtherCalls();

    rotate(ROTATE_CLOCKWISE, 1000);
}

TEST(NavigationTests, RotateClockwise90DegInitializesClockwiseRotation)
{
    struct mouse_physical_params mouse_params{};
    mouse_params.wheel_diameter_mm = 32.0;
    mouse_params.wheel_base_mm = 90.0;
    mouse_params.encoder_events_per_revolution = 60.0;
    mouse_params.motor_pinion_gear_teeth = 13.0;
    mouse_params.wheel_gear_teeth = 44.0;

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    mock().ignoreOtherCalls();

    rotate_clockwise_90_deg();
}

TEST(NavigationTests, RotateCounterClockwise90DegInitializesCounterClockwiseRotation)
{
    struct mouse_physical_params mouse_params{};
    mouse_params.wheel_diameter_mm = 32.0;
    mouse_params.wheel_base_mm = 90.0;
    mouse_params.encoder_events_per_revolution = 60.0;
    mouse_params.motor_pinion_gear_teeth = 13.0;
    mouse_params.wheel_gear_teeth = 44.0;

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

    mock().ignoreOtherCalls();

    rotate_counter_clockwise_90_deg();
}

TEST(NavigationTests, Rotate180DegInitializesClockwiseRotation)
{
    struct mouse_physical_params mouse_params{};
    mouse_params.wheel_diameter_mm = 32.0;
    mouse_params.wheel_base_mm = 90.0;
    mouse_params.encoder_events_per_revolution = 60.0;
    mouse_params.motor_pinion_gear_teeth = 13.0;
    mouse_params.wheel_gear_teeth = 44.0;

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    mock().ignoreOtherCalls();

    rotate_180_deg();
}
