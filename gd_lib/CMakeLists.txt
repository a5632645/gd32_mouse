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