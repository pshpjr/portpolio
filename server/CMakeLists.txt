cmake_minimum_required(VERSION 3.23)

project(portpolio VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PORTPOLIO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(portpolio_server STATIC ${PORTPOLIO_SOURCES})
add_library(portpolio::server ALIAS portpolio_server)

target_include_directories(portpolio_server
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(portpolio_server PUBLIC cxx_std_20)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(portpolio_unit_tests
        tests/unit/scaffold_smoke_test.cpp
    )

    target_link_libraries(portpolio_unit_tests
        PRIVATE
            portpolio::server
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(portpolio_unit_tests)
endif()
