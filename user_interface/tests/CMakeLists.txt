#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for user_interface test files                            #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_user_interface.cpp
    ${CMAKE_SOURCE_DIR}/src/user_interface/user_interface.c
    ${CMAKE_SOURCE_DIR}/src/user_interface/user_interface.h
)

add_executable(test_user_interface
    ${CMAKE_CURRENT_SOURCE_DIR}/test_user_interface.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_user_interface PRIVATE
    user_interface_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_user_interface COMMAND test_user_interface)
