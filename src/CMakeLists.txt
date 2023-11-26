cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC_FILES
        "*.h"
        "*.hpp"
        "*.cpp"
        "*.cc"
        )

message("Building MDCII with CMake version: ${CMAKE_VERSION}")

find_package(spdlog CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(MDCII ${SRC_FILES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("-- USE MDCII DEBUG SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC MDCII_DEBUG_BUILD SPDLOG_NO_EXCEPTIONS)
else()
    message("-- USE MDCII RELEASE SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_NO_EXCEPTIONS)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

# todo: olc with win64

# linux
target_link_libraries(${PROJECT_NAME} PRIVATE GL)
target_link_libraries(${PROJECT_NAME} PRIVATE png)
target_link_libraries(${PROJECT_NAME} PRIVATE X11)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE pulse-simple)
target_link_libraries(${PROJECT_NAME} PRIVATE pulse)

# copy config.ini
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/config.ini $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# copy resources folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
