#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for initialization test files                            #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_initialization.cpp
    ${CMAKE_SOURCE_DIR}/src/initialization/initialization.c
    ${CMAKE_SOURCE_DIR}/src/initialization/initialization.h
)

add_executable(test_initialization
    ${CMAKE_CURRENT_SOURCE_DIR}/test_initialization.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_initialization PRIVATE
    initialization_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_initialization COMMAND test_initialization)
