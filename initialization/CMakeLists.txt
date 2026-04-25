#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for initialization library source files                  #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(initialization_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/initialization.c
)

target_include_directories(initialization_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(initialization_lib PUBLIC
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
