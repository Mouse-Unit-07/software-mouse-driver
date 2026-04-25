#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for navigation library source files                      #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(navigation_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/navigation.c
)

target_include_directories(navigation_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(navigation_lib PUBLIC
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
