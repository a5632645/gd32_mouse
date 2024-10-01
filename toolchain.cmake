#设置 CMake 最低支持版本
cmake_minimum_required(VERSION 3.17)
#Cmake 交叉编译配置
SET(CMAKE_SYSTEM_NAME Generic)
#设置支持 ASM
# ENABLE_LANGUAGE(ASM)

#设置 C 编译工具
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
#ELF 转换为 bin 和 hex 文件工具
SET(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#查看文件大小工具
SET(CMAKE_SIZE arm-none-eabi-size)
