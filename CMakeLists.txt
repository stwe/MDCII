cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE cmake/conan_toolchain.cmake)

project(MDCII)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
