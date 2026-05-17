#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for user_request test files                              #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_user_request.cpp
    ${CMAKE_SOURCE_DIR}/src/user_request/user_request.c
    ${CMAKE_SOURCE_DIR}/src/user_request/user_request.h
)

add_executable(test_user_request
    ${CMAKE_CURRENT_SOURCE_DIR}/test_user_request.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_user_request PRIVATE
    user_request_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_user_request COMMAND test_user_request)
