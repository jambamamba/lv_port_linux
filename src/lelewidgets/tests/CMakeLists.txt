cmake_minimum_required(VERSION 3.10)

enable_testing()

add_executable(style_tests lelestyle.cpp)
target_link_libraries(style_tests gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(style_tests)