cmake_minimum_required(VERSION 3.23)

project(portpolio_lib VERSION 0.1.0 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include(CTest)

find_package(strong_type CONFIG REQUIRED)

add_library(portpolio_lib INTERFACE)
add_library(psh::lib ALIAS portpolio_lib)

target_include_directories(portpolio_lib
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(portpolio_lib INTERFACE cxx_std_23)

target_link_libraries(portpolio_lib
    INTERFACE
        strong_type::strong_type
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(portpolio_lib_unit_tests
        tests/clock/clock_test.cpp
        tests/types/not_null_test.cpp
        tests/types/strong_id_test.cpp
        tests/utils/container_utils_test.cpp
        tests/utils/game_random_test.cpp
        tests/utils/random_utils_test.cpp
        tests/utils/string_utils_test.cpp
    )

    target_link_libraries(portpolio_lib_unit_tests
        PRIVATE
            psh::lib
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(portpolio_lib_unit_tests)
endif()
