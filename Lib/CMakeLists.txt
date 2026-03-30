cmake_minimum_required(VERSION 3.23)

project(portpolio_lib VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

find_package(Boost CONFIG REQUIRED COMPONENTS asio system)
find_package(concurrentqueue CONFIG REQUIRED)
find_package(strong_type CONFIG REQUIRED)

add_library(portpolio_lib
    include/network/acceptor.cpp
    include/network/connector.cpp
    include/network/io_handler.cpp
    include/network/packet.cpp
    include/network/session.cpp
)
add_library(psh::lib ALIAS portpolio_lib)

target_include_directories(portpolio_lib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(portpolio_lib PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(portpolio_lib PUBLIC /EHsc)
endif()

if(WIN32)
    target_compile_definitions(portpolio_lib PUBLIC _WIN32_WINNT=0x0A00)
endif()

target_link_libraries(portpolio_lib
    PUBLIC
        Boost::system
        Boost::asio
        concurrentqueue::concurrentqueue
        strong_type::strong_type
)

if(WIN32)
    target_link_libraries(portpolio_lib PUBLIC ws2_32 mswsock)
endif()

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(portpolio_lib_unit_tests
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
