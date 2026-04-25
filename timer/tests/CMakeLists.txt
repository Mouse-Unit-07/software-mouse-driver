#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for timer test files                                     #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_timer.cpp
    ${CMAKE_SOURCE_DIR}/src/timer/timer.c
    ${CMAKE_SOURCE_DIR}/src/timer/timer.h
    PARENT_SCOPE
)

add_executable(test_timer
    ${CMAKE_CURRENT_SOURCE_DIR}/test_timer.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_timer PRIVATE
    timer_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_timer COMMAND test_timer)
