#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for navigation test files                                #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_navigation.cpp
    ${CMAKE_SOURCE_DIR}/src/navigation/navigation.c
    ${CMAKE_SOURCE_DIR}/src/navigation/navigation.h
)

add_executable(test_navigation
    ${CMAKE_CURRENT_SOURCE_DIR}/test_navigation.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_navigation PRIVATE
    navigation_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_navigation COMMAND test_navigation)
