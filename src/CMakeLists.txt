cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES
        "*.h"
        "*.hpp"
        "*.cpp"
        "*.cc"
        )

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)

message("Building with CMake version: ${CMAKE_VERSION}")

add_executable(MDCII ${SRC_FILES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("-- USE DEBUG SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC MDCII_DEBUG_BUILD GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL SPDLOG_NO_EXCEPTIONS)
else()
    message("-- USE RELEASE SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL SPDLOG_NO_EXCEPTIONS)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE magic_enum::magic_enum)
target_link_libraries(${PROJECT_NAME} PRIVATE freetype)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::protobuf protobuf::libprotoc protobuf::libprotobuf)

# copy config.ini
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/config.ini $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# copy resources folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
