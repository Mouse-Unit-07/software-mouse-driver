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
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(global_time_lib PRIVATE
    device_drivers_lib
)
