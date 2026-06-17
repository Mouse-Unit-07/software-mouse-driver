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
uint32_t fake_ir_1_reading_value{0u};
uint32_t fake_ir_2_reading_value{0u};
uint32_t fake_ir_3_reading_value{0u};
uint32_t fake_ir_4_reading_value{0u};

uint32_t read_ir_1_sensor(void)
{
    return fake_ir_1_reading_value;
}

uint32_t read_ir_2_sensor(void)
{
    return fake_ir_2_reading_value;
}

uint32_t read_ir_3_sensor(void)
{
    return fake_ir_3_reading_value;
}

uint32_t read_ir_4_sensor(void)
{
    return fake_ir_4_reading_value;
}

void reset_ir_mocks(void)
{
    fake_ir_1_reading_value = 0u;
    fake_ir_2_reading_value = 0u;
    fake_ir_3_reading_value = 0u;
    fake_ir_4_reading_value = 0u;
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
constexpr double FLOAT_TOLERANCE{1e-6};

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

struct maze_physical_params create_maze_physical_params(void)
{
    struct maze_physical_params params{0};
    params.post_size_mm = 168.0;
    params.wall_size_mm = 12.0;

    return params;
}

struct mouse_physical_params create_mouse_physical_params(void)
{
    struct mouse_physical_params params{0};
    params.wheel_diameter_mm = 32.0;
    params.wheel_base_mm = 90.0;
    params.encoder_events_per_revolution = 60.0;
    params.motor_pinion_gear_teeth = 13.0;
    params.wheel_gear_teeth = 44.0;

    return params;
}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(NavigationTests)
{
    void setup() override
    {
        mock().clear();
        init_navigation();
        reset_encoder_mocks();
        reset_ir_mocks();
        set_move_forward_control_configs();
        set_rotation_control_config();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
        deinit_navigation();
    }
};

/*============================================================================*/
/*                                    Tests                                   */
/*============================================================================*/
TEST(NavigationTests, InitNavigationClearsNavigationParameters)
{
    struct mouse_physical_params mouse{create_mouse_physical_params()};
    struct maze_physical_params maze{create_maze_physical_params()};

    calculate_mouse_params(mouse);
    calculate_maze_params(maze);
    calculate_navigation_params();

    init_navigation();

    struct mouse_physical_params expected_mouse_phys{};
    struct mouse_calculated_params expected_mouse_calc{};
    struct maze_physical_params expected_maze_phys{};
    struct maze_calculated_params expected_maze_calc{};
    struct navigation_params expected_nav{};

    struct mouse_physical_params actual_mouse_phys = get_mouse_physical_params();
    struct mouse_calculated_params actual_mouse_calc = get_mouse_calculated_params();
    struct maze_physical_params actual_maze_phys = get_maze_physical_params();
    struct maze_calculated_params actual_maze_calc = get_maze_calculated_params();
    struct navigation_params actual_nav = get_navigation_params();

    MEMCMP_EQUAL(&expected_mouse_phys, &actual_mouse_phys, sizeof(expected_mouse_phys));
    MEMCMP_EQUAL(&expected_mouse_calc, &actual_mouse_calc, sizeof(expected_mouse_calc));
    MEMCMP_EQUAL(&expected_maze_phys, &actual_maze_phys, sizeof(expected_maze_phys));
    MEMCMP_EQUAL(&expected_maze_calc, &actual_maze_calc, sizeof(expected_maze_calc));
    MEMCMP_EQUAL(&expected_nav, &actual_nav, sizeof(expected_nav));
}

TEST(NavigationTests, InitNavigationClearsMoveForwardConfigs)
{
    struct move_forward_control_config cfg{create_default_control_config()};

    set_no_wall_move_forward_control_config(cfg);
    set_one_wall_move_forward_control_config(cfg);
    set_both_wall_move_forward_control_config(cfg);

    init_navigation();

    struct move_forward_control_config no_wall = get_no_wall_move_forward_control_config();
    struct move_forward_control_config one_wall = get_one_wall_move_forward_control_config();
    struct move_forward_control_config both_wall = get_both_wall_move_forward_control_config();

    struct move_forward_control_config expected = {0};

    MEMCMP_EQUAL(&expected, &no_wall, sizeof(no_wall));
    MEMCMP_EQUAL(&expected, &one_wall, sizeof(one_wall));
    MEMCMP_EQUAL(&expected, &both_wall, sizeof(both_wall));
}

TEST(NavigationTests, InitNavigationClearsRotateConfig)
{
    struct rotate_control_config cfg{create_default_rotate_control_config()};

    set_rotate_control_config(cfg);

    init_navigation();

    struct rotate_control_config actual = get_rotate_control_config();
    struct rotate_control_config expected = {0};

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, InitNavigationClearsMoveForwardStatistics)
{
    struct move_forward_statistics stats = get_move_forward_statistics();

    struct move_forward_statistics expected{0};

    MEMCMP_EQUAL(&expected, &stats, sizeof(stats));
}

TEST(NavigationTests, InitNavigationClearsRotateStatistics)
{
    struct rotate_statistics stats = get_rotate_statistics();

    struct rotate_statistics expected{0};

    MEMCMP_EQUAL(&expected, &stats, sizeof(stats));
}

TEST(NavigationTests, InitNavigationClearsMoveForwardCalculatedParams)
{
    struct move_forward_control_config cfg{};
    cfg.base_speed = 100;

    set_no_wall_move_forward_control_config(cfg);
    set_one_wall_move_forward_control_config(cfg);
    set_both_wall_move_forward_control_config(cfg);

    init_navigation();

    struct move_forward_calculated_params no_wall{get_no_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params one_wall{get_one_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params both_wall{get_both_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params expected{0};

    MEMCMP_EQUAL(&expected, &no_wall, sizeof(expected));
    MEMCMP_EQUAL(&expected, &one_wall, sizeof(expected));
    MEMCMP_EQUAL(&expected, &both_wall, sizeof(expected));
}

TEST(NavigationTests, DeinitNavigationClearsNavigationParameters)
{
    struct mouse_physical_params mouse{create_mouse_physical_params()};
    struct maze_physical_params maze{create_maze_physical_params()};

    calculate_mouse_params(mouse);
    calculate_maze_params(maze);
    calculate_navigation_params();

    deinit_navigation();

    struct mouse_physical_params expected_mouse_phys{};
    struct mouse_calculated_params expected_mouse_calc{};
    struct maze_physical_params expected_maze_phys{};
    struct maze_calculated_params expected_maze_calc{};
    struct navigation_params expected_nav{};

    struct mouse_physical_params actual_mouse_phys = get_mouse_physical_params();
    struct mouse_calculated_params actual_mouse_calc = get_mouse_calculated_params();
    struct maze_physical_params actual_maze_phys = get_maze_physical_params();
    struct maze_calculated_params actual_maze_calc = get_maze_calculated_params();
    struct navigation_params actual_nav = get_navigation_params();

    MEMCMP_EQUAL(&expected_mouse_phys, &actual_mouse_phys, sizeof(expected_mouse_phys));
    MEMCMP_EQUAL(&expected_mouse_calc, &actual_mouse_calc, sizeof(expected_mouse_calc));
    MEMCMP_EQUAL(&expected_maze_phys, &actual_maze_phys, sizeof(expected_maze_phys));
    MEMCMP_EQUAL(&expected_maze_calc, &actual_maze_calc, sizeof(expected_maze_calc));
    MEMCMP_EQUAL(&expected_nav, &actual_nav, sizeof(expected_nav));
}

TEST(NavigationTests, DeinitNavigationClearsMoveForwardConfigs)
{
    struct move_forward_control_config cfg{create_default_control_config()};

    set_no_wall_move_forward_control_config(cfg);
    set_one_wall_move_forward_control_config(cfg);
    set_both_wall_move_forward_control_config(cfg);

    deinit_navigation();

    struct move_forward_control_config no_wall = get_no_wall_move_forward_control_config();
    struct move_forward_control_config one_wall = get_one_wall_move_forward_control_config();
    struct move_forward_control_config both_wall = get_both_wall_move_forward_control_config();

    struct move_forward_control_config expected = {0};

    MEMCMP_EQUAL(&expected, &no_wall, sizeof(no_wall));
    MEMCMP_EQUAL(&expected, &one_wall, sizeof(one_wall));
    MEMCMP_EQUAL(&expected, &both_wall, sizeof(both_wall));
}

TEST(NavigationTests, DeinitNavigationClearsRotateConfig)
{
    struct rotate_control_config cfg{create_default_rotate_control_config()};

    set_rotate_control_config(cfg);

    deinit_navigation();

    struct rotate_control_config actual = get_rotate_control_config();
    struct rotate_control_config expected = {0};

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, DeinitNavigationClearsMoveForwardCalculatedParams)
{
    struct move_forward_control_config cfg{};
    cfg.base_speed = 100;

    set_no_wall_move_forward_control_config(cfg);
    set_one_wall_move_forward_control_config(cfg);
    set_both_wall_move_forward_control_config(cfg);

    deinit_navigation();

    struct move_forward_calculated_params no_wall{get_no_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params one_wall{get_one_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params both_wall{get_both_wall_move_forward_calculated_params()};
    struct move_forward_calculated_params expected{0};

    MEMCMP_EQUAL(&expected, &no_wall, sizeof(expected));
    MEMCMP_EQUAL(&expected, &one_wall, sizeof(expected));
    MEMCMP_EQUAL(&expected, &both_wall, sizeof(expected));
}

TEST(NavigationTests, CalculateMouseParamsStoresPhysicalParameters)
{
    struct mouse_physical_params expected{create_mouse_physical_params()};

    calculate_mouse_params(expected);

    struct mouse_physical_params actual = get_mouse_physical_params();

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, CalculateMazeParamsStoresPhysicalParameters)
{
    struct maze_physical_params expected{create_maze_physical_params()};

    calculate_maze_params(expected);

    struct maze_physical_params actual = get_maze_physical_params();

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, CalculateMouseParamsCalculatesDerivedParameters)
{
    struct mouse_physical_params p{create_mouse_physical_params()};

    calculate_mouse_params(p);

    struct mouse_calculated_params actual = get_mouse_calculated_params();

    DOUBLES_EQUAL(13.0 / 44.0, actual.gear_ratio, FLOAT_TOLERANCE);
}

TEST(NavigationTests, CalculateMazeParamsCalculatesCellSize)
{
    struct maze_physical_params p{create_maze_physical_params()};

    calculate_maze_params(p);

    struct maze_calculated_params actual = get_maze_calculated_params();

    DOUBLES_EQUAL(180.0, actual.cell_size_mm, FLOAT_TOLERANCE);
}

TEST(NavigationTests, CalculateNavigationParamsCalculatesTargets)
{
    struct mouse_physical_params mouse{create_mouse_physical_params()};
    struct maze_physical_params maze{create_maze_physical_params()};

    calculate_mouse_params(mouse);
    calculate_maze_params(maze);
    calculate_navigation_params();

    struct navigation_params nav = get_navigation_params();

    LONGS_EQUAL(91, nav.move_forward_one_cell_target_ticks);
    LONGS_EQUAL(36, nav.rotate_90_degree_target_ticks);
    LONGS_EQUAL(71, nav.rotate_180_degree_target_ticks);
}

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

    LONGS_EQUAL(-50, errors.ir_error);
}

TEST(NavigationTests, CalculateMoveForwardErrorsRightWallModeCalculatesIrError)
{
    struct move_forward_state state{0};

    fake_ir_3_reading_value = 450;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_RIGHT, 500u)};

    LONGS_EQUAL(50, errors.ir_error);
}

TEST(NavigationTests, CalculateMoveForwardErrorsBothWallModeCalculatesIrError)
{
    struct move_forward_state state{0};

    fake_ir_2_reading_value = 400;
    fake_ir_3_reading_value = 550;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_BOTH, 0u)};

    LONGS_EQUAL(-150, errors.ir_error);
}

TEST(NavigationTests, CalculateMoveForwardErrorsCalculatesIrDerivative)
{
    struct move_forward_state state{0};
    state.prev_ir_error = 10;

    fake_ir_2_reading_value = 450;

    struct move_forward_errors errors{
        calculate_move_forward_errors(&state, WALL_FEEDBACK_LEFT, 500u)};

    LONGS_EQUAL(-50, errors.ir_error);
    LONGS_EQUAL(-60, errors.ir_derivative);
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

TEST(NavigationTests, ResetMoveForwardErrorHistoryClearsDerivativeState)
{
    struct move_forward_state state{0};

    state.prev_velocity_error = 10;
    state.prev_angle_error = 20;
    state.prev_ir_error = 30;

    fake_encoder_1_ticks = 100;
    fake_encoder_2_ticks = 200;

    reset_move_forward_error_history(&state);

    LONGS_EQUAL(0, state.prev_velocity_error);
    LONGS_EQUAL(0, state.prev_angle_error);
    LONGS_EQUAL(0, state.prev_ir_error);

    LONGS_EQUAL(100, state.prev_enc_1_ticks);
    LONGS_EQUAL(200, state.prev_enc_2_ticks);
}

TEST(NavigationTests, MoveForwardStopsMotorsWhenMaxStepsExceeded)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};
    struct maze_physical_params maze_params{create_maze_physical_params()};

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

TEST(NavigationTests, SetMoveForwardControlConfigFunctionsCalculateDriftTicks)
{
    struct move_forward_control_config cfg{};
    cfg.base_speed = 100;

    set_no_wall_move_forward_control_config(cfg);
    set_one_wall_move_forward_control_config(cfg);
    set_both_wall_move_forward_control_config(cfg);

    LONGS_EQUAL(18, get_no_wall_move_forward_calculated_params().drift_ticks);
    LONGS_EQUAL(18, get_one_wall_move_forward_calculated_params().drift_ticks);
    LONGS_EQUAL(18, get_both_wall_move_forward_calculated_params().drift_ticks);
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

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

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

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

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

TEST(NavigationTests, CalculateRotateMotorOutputZeroErrorUsesBaseSpeed)
{
    struct rotate_errors errors{0};
    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 200);
    CHECK(output.motor_2_speed == 200);
}

TEST(NavigationTests, CalculateRotateMotorOutputPositiveControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = 10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed > output.motor_2_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputNegativeControl)
{
    struct rotate_errors errors{0};
    errors.velocity_error = -10;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, cfg)};

    CHECK(output.motor_2_speed > output.motor_1_speed);
}

TEST(NavigationTests, CalculateRotateMotorOutputClampsMaximum)
{
    struct rotate_errors errors{0};
    errors.velocity_error = 100000;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 255);
}

TEST(NavigationTests, CalculateRotateMotorOutputClampsMinimum)
{
    struct rotate_errors errors{0};
    errors.velocity_error = -100000;

    struct rotate_control_config cfg{create_default_rotate_control_config()};
    struct motor_output output{calculate_rotate_motor_output(errors, cfg)};

    CHECK(output.motor_1_speed == 100);
}

TEST(NavigationTests, RotateStopsMotorsWhenMaxStepsExceeded)
{
    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

    mock().ignoreOtherCalls();

    rotate(ROTATE_CLOCKWISE, 1000);
}

TEST(NavigationTests, RotateClockwise90DegInitializesClockwiseRotation)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

    mock().ignoreOtherCalls();

    rotate_clockwise_90_deg();
}

TEST(NavigationTests, RotateCounterClockwise90DegInitializesCounterClockwiseRotation)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_backward");
    mock().expectOneCall("set_wheel_motor_2_direction_forward");

    mock().ignoreOtherCalls();

    rotate_counter_clockwise_90_deg();
}

TEST(NavigationTests, Rotate180DegInitializesClockwiseRotation)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};

    calculate_mouse_params(mouse_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().expectOneCall("clear_1_encoder_ticks");
    mock().expectOneCall("clear_2_encoder_ticks");

    mock().expectOneCall("set_wheel_motor_1_direction_forward");
    mock().expectOneCall("set_wheel_motor_2_direction_backward");

    mock().ignoreOtherCalls();

    rotate_180_deg();
}

/*----------------------------------------------------------------------------*/
/* side-wall detection */
TEST(NavigationTests, InitSideWallDetectorClearsDetector)
{
    struct side_wall_detector detector{};

    detector.have_previous_reading = true;
    detector.prev_left_reading = 123;
    detector.prev_right_reading = 456;
    detector.left_first_change_recorded = true;
    detector.right_first_change_recorded = true;
    detector.left_wall_present_on_first_change = true;
    detector.right_wall_present_on_first_change = true;
    detector.left_wall_currently_present = true;
    detector.right_wall_currently_present = true;
    detector.left_sum = 100;
    detector.right_sum = 200;
    detector.samples_collected = 5;

    init_side_wall_detector(&detector);

    struct side_wall_detector expected{0};

    MEMCMP_EQUAL(&expected, &detector, sizeof(detector));
}

TEST(NavigationTests, SetSideWallDetectionConfigStoresConfiguration)
{
    struct side_wall_detection_config expected{};
    expected.num_detection_samples = 10;
    expected.reading_threshold = 500;
    expected.slope_threshold = 50;
    expected.reading_start_offset = 0.25;

    set_side_wall_detection_config(expected);

    struct side_wall_detection_config actual = get_side_wall_detection_config();

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, SetSideWallDetectionConfigCalculatesReadingStartOffsetTicks)
{
    calculate_mouse_params(create_mouse_physical_params());
    calculate_maze_params(create_maze_physical_params());
    calculate_navigation_params();

    struct side_wall_detection_config cfg{};

    cfg.reading_start_offset = 0.5;

    set_side_wall_detection_config(cfg);

    struct side_wall_calculated_params params = get_side_wall_calculated_params();

    LONGS_EQUAL(46u, params.reading_start_offset_ticks);
}

TEST(NavigationTests, UpdateSideWallDetectorDoesNotCollectSamplesBeforeOffset)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};
    struct maze_physical_params maze_params{create_maze_physical_params()};
    calculate_mouse_params(mouse_params);
    calculate_maze_params(maze_params);
    calculate_navigation_params();

    struct side_wall_detection_config cfg{};
    cfg.num_detection_samples = 10;
    cfg.slope_threshold = 100;
    cfg.reading_start_offset = 0.5;

    set_side_wall_detection_config(cfg);

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;
    fake_ir_2_reading_value = 400;
    fake_ir_3_reading_value = 500;

    struct side_wall_detector detector{};
    update_side_wall_detector(&detector);

    CHECK(detector.samples_collected == 0u);
}

TEST(NavigationTests, UpdateSideWallDetectorCollectsSamplesAfterOffset)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.num_detection_samples = 10;
    cfg.reading_start_offset = 0.0;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 400;
    fake_ir_3_reading_value = 500;

    update_side_wall_detector(&detector);

    CHECK(detector.samples_collected == 1u);
    CHECK(detector.left_sum == 400u);
    CHECK(detector.right_sum == 500u);
}

TEST(NavigationTests, UpdateSideWallDetectorStopsCollectingAfterMaximumSamples)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.num_detection_samples = 2;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 100;
    fake_ir_3_reading_value = 200;

    update_side_wall_detector(&detector);
    update_side_wall_detector(&detector);
    update_side_wall_detector(&detector);

    CHECK(detector.samples_collected == 2u);
    CHECK(detector.left_sum == 200u);
    CHECK(detector.right_sum == 400u);
}

TEST(NavigationTests, UpdateSideWallDetectorFirstReadingOnlyStoresPreviousValues)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 50;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 500;
    fake_ir_3_reading_value = 600;

    update_side_wall_detector(&detector);

    CHECK(detector.have_previous_reading);

    CHECK_FALSE(detector.left_first_change_recorded);
    CHECK_FALSE(detector.right_first_change_recorded);

    CHECK(detector.prev_left_reading == 500u);
    CHECK(detector.prev_right_reading == 600u);
}

TEST(NavigationTests, UpdateSideWallDetectorDetectsLeftWallAppearance)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 50;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 100;
    update_side_wall_detector(&detector);

    fake_ir_2_reading_value = 200;
    update_side_wall_detector(&detector);

    CHECK(detector.left_first_change_recorded);
    CHECK(detector.left_wall_present_on_first_change);
    CHECK(detector.left_wall_currently_present);
}

TEST(NavigationTests, UpdateSideWallDetectorDetectsRightWallAppearance)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 50;

    set_side_wall_detection_config(cfg);

    fake_ir_3_reading_value = 100;
    update_side_wall_detector(&detector);

    fake_ir_3_reading_value = 200;
    update_side_wall_detector(&detector);

    CHECK(detector.right_first_change_recorded);
    CHECK(detector.right_wall_present_on_first_change);
    CHECK(detector.right_wall_currently_present);
}

TEST(NavigationTests, UpdateSideWallDetectorDetectsLeftWallDisappearance)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 50;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 300;
    update_side_wall_detector(&detector);

    fake_ir_2_reading_value = 100;
    update_side_wall_detector(&detector);

    CHECK(detector.left_first_change_recorded);
    CHECK_FALSE(detector.left_wall_present_on_first_change);
    CHECK_FALSE(detector.left_wall_currently_present);
}

TEST(NavigationTests, UpdateSideWallDetectorDetectsRightWallDisappearance)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 50;

    set_side_wall_detection_config(cfg);

    fake_ir_3_reading_value = 300;
    update_side_wall_detector(&detector);

    fake_ir_3_reading_value = 100;
    update_side_wall_detector(&detector);

    CHECK(detector.right_first_change_recorded);
    CHECK_FALSE(detector.right_wall_present_on_first_change);
    CHECK_FALSE(detector.right_wall_currently_present);
}

TEST(NavigationTests, UpdateSideWallDetectorIgnoresChangesBelowSlopeThreshold)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.slope_threshold = 100;

    set_side_wall_detection_config(cfg);

    fake_ir_2_reading_value = 500;
    update_side_wall_detector(&detector);

    fake_ir_2_reading_value = 550;
    update_side_wall_detector(&detector);

    CHECK_FALSE(detector.left_first_change_recorded);
}

TEST(NavigationTests, DetermineWallModeUsesAverageWhenNoTransitionRecorded)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.reading_threshold = 500;

    set_side_wall_detection_config(cfg);

    detector.left_sum = 600;
    detector.right_sum = 200;
    detector.samples_collected = 1;

    CHECK(determine_wall_mode(&detector) == WALL_FEEDBACK_LEFT);
}

TEST(NavigationTests, DetermineWallModeReturnsBothWallsFromAverages)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.reading_threshold = 500;

    set_side_wall_detection_config(cfg);

    detector.left_sum = 700;
    detector.right_sum = 800;
    detector.samples_collected = 1;

    CHECK(determine_wall_mode(&detector) == WALL_FEEDBACK_BOTH);
}

TEST(NavigationTests, DetermineWallModeUsesCurrentWallStateAfterTransition)
{
    struct side_wall_detector detector{};

    detector.left_first_change_recorded = true;
    detector.left_wall_currently_present = true;

    detector.right_first_change_recorded = true;
    detector.right_wall_currently_present = false;

    CHECK(determine_wall_mode(&detector) == WALL_FEEDBACK_LEFT);
}

TEST(NavigationTests, DetermineWallPresenceReturnsWallStateAtFirstTransition)
{
    struct side_wall_detector detector{};

    detector.left_first_change_recorded = true;
    detector.left_wall_present_on_first_change = true;

    detector.left_wall_currently_present = false;

    CHECK(determine_wall_presence(&detector, true));
}

TEST(NavigationTests, DetermineWallPresenceReturnsNoWallStateAtFirstTransition)
{
    struct side_wall_detector detector{};

    detector.left_first_change_recorded = true;
    detector.left_wall_present_on_first_change = false;

    detector.left_wall_currently_present = true;

    CHECK_FALSE(determine_wall_presence(&detector, true));
}

TEST(NavigationTests, DetermineWallPresenceUsesAverageWhenNoTransitionRecorded)
{
    struct side_wall_detector detector{};

    struct side_wall_detection_config cfg{};
    cfg.reading_threshold = 500;

    set_side_wall_detection_config(cfg);

    detector.left_sum = 600;
    detector.samples_collected = 1;

    CHECK(determine_wall_presence(&detector, true));
}

TEST(NavigationTests, DetermineWallPresenceReturnsFalseWhenNoSamplesCollected)
{
    struct side_wall_detector detector{};

    CHECK_FALSE(determine_wall_presence(&detector, true));
}

/*----------------------------------------------------------------------------*/
/* front-wall detection */
TEST(NavigationTests, SetFrontWallDetectionConfigStoresConfiguration)
{
    struct front_wall_detection_config expected{};

    expected.num_detection_samples = 8;
    expected.reading_threshold = 500;

    set_front_wall_detection_config(expected);

    struct front_wall_detection_config actual = get_front_wall_detection_config();

    MEMCMP_EQUAL(&expected, &actual, sizeof(actual));
}

TEST(NavigationTests, IsFrontWallPresentReturnsFalseWhenZeroSamplesRequested)
{
    struct front_wall_detection_config cfg{};

    cfg.num_detection_samples = 0;
    cfg.reading_threshold = 500;

    set_front_wall_detection_config(cfg);

    CHECK_FALSE(is_front_wall_present());
}

TEST(NavigationTests, IsFrontWallPresentReturnsTrueWhenAverageExceedsThreshold)
{
    struct front_wall_detection_config cfg{};
    cfg.num_detection_samples = 4;
    cfg.reading_threshold = 500;

    set_front_wall_detection_config(cfg);

    fake_ir_1_reading_value = 600;
    fake_ir_4_reading_value = 700;

    CHECK(is_front_wall_present());
}

TEST(NavigationTests, IsFrontWallPresentReturnsFalseWhenAverageBelowThreshold)
{
    struct front_wall_detection_config cfg{};
    cfg.num_detection_samples = 4;
    cfg.reading_threshold = 500;

    set_front_wall_detection_config(cfg);

    fake_ir_1_reading_value = 300;
    fake_ir_4_reading_value = 400;

    CHECK_FALSE(is_front_wall_present());
}

TEST(NavigationTests, IsFrontWallPresentReturnsTrueWhenAtThreshold)
{
    struct front_wall_detection_config cfg{};
    cfg.num_detection_samples = 4;
    cfg.reading_threshold = 500;

    set_front_wall_detection_config(cfg);

    fake_ir_1_reading_value = 500;
    fake_ir_4_reading_value = 500;

    CHECK(is_front_wall_present());
}

TEST(NavigationTests, WallPresenceFlagsDefaultToFalse)
{
    CHECK_FALSE(is_left_wall_present());
    CHECK_FALSE(is_right_wall_present());
}

/*----------------------------------------------------------------------------*/
/* telemetry */
TEST(NavigationTests, MoveForwardSetsTimeoutFlagWhenMaxStepsExceeded)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};
    struct maze_physical_params maze_params{create_maze_physical_params()};

    calculate_mouse_params(mouse_params);
    calculate_maze_params(maze_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().ignoreOtherCalls();

    move_forward();

    struct move_forward_statistics stats{get_move_forward_statistics()};

    CHECK(stats.timeout_occurred);
}

TEST(NavigationTests, RotateSetsTimeoutFlagWhenMaxStepsExceeded)
{
    struct mouse_physical_params mouse_params{create_mouse_physical_params()};
    struct maze_physical_params maze_params{create_maze_physical_params()};

    calculate_mouse_params(mouse_params);
    calculate_maze_params(maze_params);
    calculate_navigation_params();

    fake_encoder_1_ticks = 0;
    fake_encoder_2_ticks = 0;

    mock().ignoreOtherCalls();

    rotate(ROTATE_CLOCKWISE, 1000);

    struct rotate_statistics stats{get_rotate_statistics()};

    CHECK(stats.timeout_occurred);
}
