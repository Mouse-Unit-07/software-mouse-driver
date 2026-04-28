#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for global_time test files                               #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_global_time.cpp
    ${CMAKE_SOURCE_DIR}/src/global_time/global_time.c
    ${CMAKE_SOURCE_DIR}/src/global_time/global_time.h
    PARENT_SCOPE
)

add_executable(test_global_time
    ${CMAKE_CURRENT_SOURCE_DIR}/test_global_time.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_global_time PRIVATE
    global_time_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_global_time COMMAND test_global_time)
