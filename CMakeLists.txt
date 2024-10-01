cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project("mouse" LANGUAGES C ASM)

# TODO: 设置时钟
add_compile_definitions(

)
# TODO: mcu型号（我草泥马）
add_compile_definitions(
)

add_subdirectory(gd_lib)
add_subdirectory(src)