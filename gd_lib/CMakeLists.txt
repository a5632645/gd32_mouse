file(GLOB_RECURSE CMSIS_SRC "CMSIS/*.c")
file(GLOB_RECURSE USBD_LIB "GD32F10x_usbd_library/*.c")
file(GLOB_RECURSE USBFS_LIB "GD32F10x_usbfs_library/*.c")
file(GLOB_RECURSE PERIPH_LIB "GD32F10x_standard_peripheral/*.c")

add_library(gd_lib
${CMSIS_SRC}
${PERIPH_LIB})

target_include_directories(gd_lib PUBLIC
.
CMSIS/GD/GD32F10x/Include
CMSIS
GD32F10x_standard_peripheral/Include)

target_link_options(gd_lib PRIVATE
    # "--specs=nano.specs"
    "-specs=nosys.specs"
    "-T${LINKER_SCRIPT}"
    "-Wl,-Map=${PROJECT_BINARY_DIR}/${PRJ_NAME}.map,--cref"
    "-Wl,--gc-sections"
)
target_compile_options(gd_lib PRIVATE
    "-mcpu=cortex-m3"
    "-mthumb"
    "-mno-thumb-interwork"
    "-mfpu=vfp"
    # "-msoft-float"
    # "-mfix-cortex-m3-ldrd"
    # "--specs=nano.specs"
    "-specs=nosys.specs"
    "-fno-exceptions" # 不使用异常，就不用管arm异常两个段了(
)