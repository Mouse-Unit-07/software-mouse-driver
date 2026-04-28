#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for time library source files                            #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(time_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/time.c
)

target_include_directories(time_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(time_lib PUBLIC
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
