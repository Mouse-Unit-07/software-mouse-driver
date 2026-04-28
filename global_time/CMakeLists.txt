#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for global_time library source files                     #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(global_time_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/global_time.c
)

target_include_directories(global_time_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(global_time_lib PUBLIC
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
