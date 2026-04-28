#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for fault_detector library source files                  #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(fault_detector_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/fault_detector.c
)

target_include_directories(fault_detector_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(fault_detector_lib PUBLIC
    battery_comparator_lib
    infrared_sensor_lib
    led_lib
    magnetic_encoder_lib
    power_enabler_lib
    processor_lib
    pushbutton_lib
    vacuum_lib
    wheel_motor_lib
    runtime_diagnostics_lib
)
