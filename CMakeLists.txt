cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project("mouse" LANGUAGES C ASM)

add_subdirectory(gd_lib)
add_subdirectory(src)