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
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(device_self_tests_lib PRIVATE
    device_drivers_lib
)
