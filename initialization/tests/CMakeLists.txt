#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for initialization test files                            #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_initialization.cpp
    ${CMAKE_SOURCE_DIR}/src/initialization/initialization.c
    ${CMAKE_SOURCE_DIR}/src/initialization/initialization.h
    PARENT_SCOPE
)

add_executable(test_initialization
    ${CMAKE_CURRENT_SOURCE_DIR}/test_initialization.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_initialization PRIVATE
    initialization_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_initialization COMMAND test_initialization)
