#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for device_self_tests library source files               #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(device_self_tests_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/device_self_tests.c
)

target_include_directories(device_self_tests_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(device_self_tests_lib PUBLIC
    battery_comparator_lib
    infrared_sensor_lib
    led_lib
    magnetic_encoder_lib
    power_enabler_lib
    processor_lib
    pushbutton_lib
    vacuum_lib
    wheel_motor_lib
)
