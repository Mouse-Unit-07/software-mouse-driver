#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# Top level CMakeLists.txt file (for this software repo)                       #
#                                                                              #
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 4.0.2)
project(mouse_driver_project LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    device_drivers
    GIT_REPOSITORY git@github.com:Mouse-Unit-07/software-device-drivers.git
    GIT_TAG main
)
FetchContent_MakeAvailable(device_drivers)

add_subdirectory(device_self_tests)
add_subdirectory(fault_detector)
add_subdirectory(initialization)
add_subdirectory(navigation)
add_subdirectory(global_time)
add_subdirectory(user_interface)

if(TARGET_WINDOWS AND SUPPORTS_WINDOWS)
    if(ENABLE_MOUSE_DRIVER_TESTS)
        add_subdirectory(device_self_tests/tests)
        add_subdirectory(fault_detector/tests)
        add_subdirectory(initialization/tests)
        add_subdirectory(navigation/tests)
        add_subdirectory(global_time/tests)
        add_subdirectory(user_interface/tests)
    endif()
endif()

add_library(mouse_driver_lib INTERFACE)

target_link_libraries(mouse_driver_lib INTERFACE
    device_self_tests_lib
    fault_detector_lib
    initialization_lib
    navigation_lib
    global_time_lib
    user_interface_lib
)
