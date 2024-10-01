file(GLOB_RECURSE CMSIS_SRC "CMSIS/*.c")
file(GLOB_RECURSE PERIPH_LIB "GD32F10x_standard_peripheral/*.c")

file(GLOB USBD_LIB1 "GD32F10x_usbd_library/device/source/*.c")
file(GLOB USBD_LIB2 "GD32F10x_usbd_library/usbd/source/*.c")
set(USBD_LIB ${USBD_LIB1} ${USBD_LIB2})

add_library(gd_lib
    ${CMSIS_SRC}
    ${PERIPH_LIB}
    ${USBD_LIB}
)

target_include_directories(gd_lib PUBLIC
.
CMSIS/GD/GD32F10x/Include
CMSIS
GD32F10x_standard_peripheral/Include)

target_include_directories(gd_lib PUBLIC
    # GD32F10x_usbd_library/class/device/audio/include
    # GD32F10x_usbd_library/class/device/cdc/include
    # GD32F10x_usbd_library/class/device/dfu/include
    GD32F10x_usbd_library/class/device/hid/include
    # GD32F10x_usbd_library/class/device/iap/include
    # GD32F10x_usbd_library/class/device/msc/include
    # GD32F10x_usbd_library/class/device/printer/include
    GD32F10x_usbd_library/device/include
    GD32F10x_usbd_library/usbd/include
)