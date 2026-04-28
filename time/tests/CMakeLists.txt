#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for time test files                                      #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_time.cpp
    ${CMAKE_SOURCE_DIR}/src/time/time.c
    ${CMAKE_SOURCE_DIR}/src/time/time.h
    PARENT_SCOPE
)

add_executable(test_time
    ${CMAKE_CURRENT_SOURCE_DIR}/test_time.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_time PRIVATE
    time_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_time COMMAND test_time)
