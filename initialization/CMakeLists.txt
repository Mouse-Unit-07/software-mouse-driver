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
    $<TARGET_PROPERTY:device_drivers_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(initialization_lib PRIVATE
    device_drivers_lib
)
