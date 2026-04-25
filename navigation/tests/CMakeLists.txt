#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for navigation test files                                #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_navigation.cpp
    ${CMAKE_SOURCE_DIR}/src/navigation/navigation.c
    ${CMAKE_SOURCE_DIR}/src/navigation/navigation.h
    PARENT_SCOPE
)

add_executable(test_navigation
    ${CMAKE_CURRENT_SOURCE_DIR}/test_navigation.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_navigation PRIVATE
    navigation_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_navigation COMMAND test_navigation)
