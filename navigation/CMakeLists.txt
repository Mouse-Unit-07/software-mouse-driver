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
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(navigation_lib PRIVATE
    device_drivers_lib
)
