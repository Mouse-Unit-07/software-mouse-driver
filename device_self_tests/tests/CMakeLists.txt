#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for device_self_tests test files                         #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_device_self_tests.cpp
    ${CMAKE_SOURCE_DIR}/src/device_self_tests/device_self_tests.c
    ${CMAKE_SOURCE_DIR}/src/device_self_tests/device_self_tests.h
    PARENT_SCOPE
)

add_executable(test_device_self_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/test_device_self_tests.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_device_self_tests PRIVATE
    device_self_tests_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_device_self_tests COMMAND test_device_self_tests)
