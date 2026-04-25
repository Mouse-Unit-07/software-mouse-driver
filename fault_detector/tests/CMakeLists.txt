#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for fault_detector test files                            #
#                                                                              #
#------------------------------------------------------------------------------#
set(ALL_TEST_SOURCES
    ${ALL_TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_fault_detector.cpp
    ${CMAKE_SOURCE_DIR}/src/fault_detector/fault_detector.c
    ${CMAKE_SOURCE_DIR}/src/fault_detector/fault_detector.h
    PARENT_SCOPE
)

add_executable(test_fault_detector
    ${CMAKE_CURRENT_SOURCE_DIR}/test_fault_detector.cpp
    ${CMAKE_SOURCE_DIR}/cmake/tests/test_main.cpp
)

target_link_libraries(test_fault_detector PRIVATE
    fault_detector_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_fault_detector COMMAND test_fault_detector)
