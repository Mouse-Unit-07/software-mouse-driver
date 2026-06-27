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
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(user_interface_lib PRIVATE
    device_drivers_lib
)
