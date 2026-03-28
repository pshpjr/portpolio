cmake_minimum_required(VERSION 3.23)

project(portpolio_lib VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_library(portpolio_lib INTERFACE)
add_library(psh::lib ALIAS portpolio_lib)

target_include_directories(portpolio_lib
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(portpolio_lib INTERFACE cxx_std_20)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(portpolio_lib_unit_tests
        tests/types/not_null_test.cpp
    )

    target_link_libraries(portpolio_lib_unit_tests
        PRIVATE
            psh::lib
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(portpolio_lib_unit_tests)
endif()
