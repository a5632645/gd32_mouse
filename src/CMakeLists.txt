file(GLOB_RECURSE SRC_LIST "*.c")
set(STARTUP_ASM "startup_gd32f10x_hd.s")

set(PRJ_NAME mouse)
add_executable(mouse.elf ${SRC_LIST} ${STARTUP_ASM})

target_link_libraries(mouse.elf PRIVATE gd_lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/src/ld.ld)

target_link_options(mouse.elf PRIVATE
    ${SYS_CALL}
    "-T${LINKER_SCRIPT}"
    "-Wl,-Map=${PROJECT_BINARY_DIR}/${PRJ_NAME}.map,--cref"
    "-Wl,--gc-sections"
)

SET(ELF_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.elf)
SET(HEX_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.hex)
SET(BIN_FILE ${PROJECT_SOURCE_DIR}/build/bin/${PRJ_NAME}.bin)

add_custom_command(TARGET "mouse.elf" POST_BUILD
 COMMAND ${CMAKE_OBJCOPY} -Obinary ${ELF_FILE} ${BIN_FILE}
 COMMAND ${CMAKE_OBJCOPY} -Oihex ${ELF_FILE} ${HEX_FILE}
 COMMENT "Building ${PRJ_NAME}.bin and ${PRJ_NAME}.hex"
 COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE}
"${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.hex"
 COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE}
"${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}.bin"
 COMMAND ${CMAKE_SIZE} --format=berkeley ${ELF_FILE} ${HEX_FILE}
 COMMENT "Invoking: Cross ARM GNU Print Size"
)