cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

#设置调试选项
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "-O3")

# -----------------------------------------------------------------------------
#Debug 模式
SET(CMAKE_BUILD_TYPE "Debug")
#Release 模式
# SET(CMAKE_BUILD_TYPE "Release")

# -----------------------------------------------------------------------------
# 系统调用
set(SYS_CALL "-specs=nosys.specs")
# set(SYS_CALL "-specs=nano.specs")

# -----------------------------------------------------------------------------
add_compile_options(
    "-mcpu=cortex-m3"
    "-mfix-cortex-m3-ldrd"
    "-mfloat-abi=softfp"
    # "-mfpu=fpv4-sp-d16"
    # "-mfpu=vfp"
    "-msoft-float"
    "-mthumb"
    "-mno-thumb-interwork"
    "-mfix-cortex-m3-ldrd"
    "-Wno-unknown-pragmas"
    ${SYS_CALL}
    "-fno-exceptions"
    "-Wimplicit-function-declaration"
    "-Werror"
)
add_link_options(
    "-mcpu=cortex-m3"
    "-mthumb"
)

# -----------------------------------------------------------------------------
# mcu型号（我草泥马）
# 时钟在gd_lib/CMSIS/GD/GD32F10x/Source/system_gd32f10x.c
add_compile_definitions(
    -DGD32F10X_HD
    -DUSE_STDPERIPH_DRIVER
)

# -----------------------------------------------------------------------------
project("mouse" LANGUAGES C ASM)

add_subdirectory(gd_lib)
add_subdirectory(src)