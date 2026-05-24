#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for user_interface library source files                  #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(user_interface_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/user_interface.c
)

target_include_directories(user_interface_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(user_interface_lib PUBLIC
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
