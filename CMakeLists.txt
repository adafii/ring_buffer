cmake_minimum_required(VERSION 3.28)
project("Multithreaded ring buffer" CXX)

option(TESTS "Enable tests" ON)

set(src_dir src)
set(tests_dir tests)

# Library

add_library(ring_buffer INTERFACE)
target_include_directories(ring_buffer INTERFACE ${src_dir})
target_compile_features(ring_buffer INTERFACE cxx_std_23)

# Tests

if (TESTS)
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG e39786088138f2749d64e9e90e0f9902daa77c40 # 1.15.0
            GIT_PROGRESS ON
    )

    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(ring_buffer_test ${tests_dir}/ring_buffer_test.cpp)
    target_link_libraries(ring_buffer_test PRIVATE GTest::gtest_main ring_buffer)

    set(compile_warnings -Wall -Wextra -Wpedantic -Wconversion)

    target_compile_features(ring_buffer_test PRIVATE cxx_std_23)
    target_compile_options(ring_buffer_test PRIVATE $<$<CONFIG:Debug>:${compile_warnings} -fsanitize=address -fsanitize=undefined>)
    target_compile_options(ring_buffer_test PRIVATE $<$<NOT:$<CONFIG:Debug>>:${compile_warnings} -flto>)

    target_link_options(ring_buffer_test PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=undefined>)
    target_link_options(ring_buffer_test PRIVATE $<$<NOT:$<CONFIG:Debug>>:-flto>)

    include(GoogleTest)
    gtest_discover_tests(ring_buffer_test)
endif ()