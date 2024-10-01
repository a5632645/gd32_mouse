#设置 CMake 最低支持版本
cmake_minimum_required(VERSION 3.17)
#Cmake 交叉编译配置
SET(CMAKE_SYSTEM_NAME Generic)
#设置支持 ASM
# ENABLE_LANGUAGE(ASM)
#Debug 模式
SET(CMAKE_BUILD_TYPE "Debug")
#Release 模式
#SET(CMAKE_BUILD_TYPE "Release")
#设置 C 编译工具
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
#ELF 转换为 bin 和 hex 文件工具
SET(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#查看文件大小工具
SET(CMAKE_SIZE arm-none-eabi-size)

# ?谁会全局定义啊，为什么库里就一定要用浮点数
#设置浮点选项
SET(MCU_FLAGS "-mcpu=cortex-m3 -mfloat-abi=softfp -mfpu=fpv4-sp-d16")
#设置警告相关信息
SET(CMAKE_C_FLAGS "${MCU_FLAGS} -w -Wno-unknown-pragmas")

#设置调试选项
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "-O3")
#添加宏定义
ADD_DEFINITIONS(
-DGD32F10X_HD-DUSE_STDPERIPH_DRIVER
)