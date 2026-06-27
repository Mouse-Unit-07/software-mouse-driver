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
    $<TARGET_PROPERTY:runtime_diagnostics_lib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(fault_detector_lib PRIVATE
    runtime_diagnostics_lib
    device_drivers_lib
)
