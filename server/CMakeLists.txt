cmake_minimum_required(VERSION 3.23)

project(portpolio VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(PORTPOLIO_SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../shared")
set(PORTPOLIO_PROTOCOL_DIR "${PORTPOLIO_SHARED_DIR}/protocol")
set(PORTPOLIO_COMBAT_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../docs/proposal/ingame/data")
set(PORTPOLIO_GENERATED_SERVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/generated")
set(PORTPOLIO_GENERATED_UNREAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../client/Source/client/Public/Generated")
set(PORTPOLIO_CODEGEN_STAMP "${CMAKE_CURRENT_BINARY_DIR}/packet_codegen.stamp")
set(PORTPOLIO_COMBAT_CODEGEN_STAMP "${CMAKE_CURRENT_BINARY_DIR}/combat_data_codegen.stamp")

file(GLOB PORTPOLIO_PROTOCOL_INPUTS CONFIGURE_DEPENDS
    "${PORTPOLIO_PROTOCOL_DIR}/*.proto"
    "${PORTPOLIO_PROTOCOL_DIR}/*.py"
    "${PORTPOLIO_PROTOCOL_DIR}/*.j2"
)

add_custom_command(
    OUTPUT "${PORTPOLIO_CODEGEN_STAMP}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PORTPOLIO_GENERATED_SERVER_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PORTPOLIO_GENERATED_UNREAL_DIR}"
    COMMAND ${Python3_EXECUTABLE}
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/packet_gen.py"
        --schema-dir "${PORTPOLIO_PROTOCOL_DIR}"
        --server-out "${PORTPOLIO_GENERATED_SERVER_DIR}"
        --unreal-out "${PORTPOLIO_GENERATED_UNREAL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${PORTPOLIO_CODEGEN_STAMP}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/packet_gen.py"
        ${PORTPOLIO_PROTOCOL_INPUTS}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating shared protocol code"
    VERBATIM
)

add_custom_target(generate_packets DEPENDS "${PORTPOLIO_CODEGEN_STAMP}")

file(GLOB PORTPOLIO_COMBAT_DATA_INPUTS CONFIGURE_DEPENDS
    "${PORTPOLIO_COMBAT_DATA_DIR}/*.schema.json"
    "${PORTPOLIO_COMBAT_DATA_DIR}/*.data.json"
)

add_custom_command(
    OUTPUT "${PORTPOLIO_COMBAT_CODEGEN_STAMP}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PORTPOLIO_GENERATED_SERVER_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PORTPOLIO_GENERATED_UNREAL_DIR}"
    COMMAND ${Python3_EXECUTABLE}
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/combat_data_gen.py"
        --schema-dir "${PORTPOLIO_COMBAT_DATA_DIR}"
        --server-out "${PORTPOLIO_GENERATED_SERVER_DIR}"
        --unreal-out "${PORTPOLIO_GENERATED_UNREAL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${PORTPOLIO_COMBAT_CODEGEN_STAMP}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/combat_data_gen.py"
        ${PORTPOLIO_COMBAT_DATA_INPUTS}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating combat data code"
    VERBATIM
)

add_custom_target(generate_combat_data DEPENDS "${PORTPOLIO_COMBAT_CODEGEN_STAMP}")
add_dependencies(generate_combat_data generate_packets)

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    "${CMAKE_CURRENT_BINARY_DIR}/lib"
)

find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PORTPOLIO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(portpolio_server STATIC ${PORTPOLIO_SOURCES})
add_library(psh::server ALIAS portpolio_server)
add_dependencies(portpolio_server generate_packets generate_combat_data)

target_include_directories(portpolio_server
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${PORTPOLIO_GENERATED_SERVER_DIR}"
)

target_compile_features(portpolio_server PUBLIC cxx_std_23)

target_link_libraries(portpolio_server
    PUBLIC
        Boost::system
        psh::lib
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(portpolio_unit_tests
        tests/unit/scaffold_smoke_test.cpp
    )

    target_link_libraries(portpolio_unit_tests
        PRIVATE
            psh::server
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(portpolio_unit_tests)
endif()
