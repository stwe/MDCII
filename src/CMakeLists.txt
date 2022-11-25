cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES
        "*.h"
        "*.hpp"
        "*.cpp"
        "*.cc"
        )

include(../conanbuildinfo.cmake)
conan_basic_setup()

add_executable(MDCII ${SRC_FILES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("-- USE DEBUG SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC MDCII_DEBUG_BUILD GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL SPDLOG_NO_EXCEPTIONS)
else()
    message("-- USE RELEASE SETUP --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL SPDLOG_NO_EXCEPTIONS)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

# copy config.ini
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/config.ini $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# copy resources folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
