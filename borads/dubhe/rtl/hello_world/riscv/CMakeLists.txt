# Works with 3.15 and tested through 3.21
cmake_minimum_required(VERSION 3.15...3.21)

# Project name and a few useful settings. Other commands can pick up the results
project(dubhe_riscv
  VERSION 0.1
  DESCRIPTION "dubhe_riscv low level program project."
  LANGUAGES C ASM
)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

set(SDK_DIR ../../../../..)

add_subdirectory(${SDK_DIR}/devices/dubhe_riscv device_dubhe_riscv.dir)

# add_subdirectory(${SDK_DIR}/hal hal.dir)

add_executable(${PROJECT_NAME}.elf )

target_sources(${PROJECT_NAME}.elf
	PRIVATE 
		src/main.c
		# ${SDK_DIR}/devices/dubhe_riscv/src/crt0.S
		# ${SDK_DIR}/devices/dubhe_riscv/src/spi_start.S
)

target_compile_options(${PROJECT_NAME}.elf
	PRIVATE
		-m32 -march=IMXpulpv2 -Wa,-march=IMXpulpv2
		-Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-pointer-to-int-cast -Wno-unused-but-set-variable
		-ffunction-sections -fdata-sections
		# -nostartfiles -nostdlib
		# -fshort-enums
		-fms-extensions
		$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
		$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
)

target_link_options( ${PROJECT_NAME}.elf
	PRIVATE
		-Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/${SDK_DIR}/devices/dubhe_riscv/src/ld/idram.ld
		-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
		-nostartfiles -nostdlib
		-Wl,--fatal-warnings -Wl,--gc-sections
		-L ${CMAKE_CURRENT_SOURCE_DIR}/${SDK_DIR}/devices/dubhe_riscv/src/ld
)

target_link_libraries( ${PROJECT_NAME}.elf
	PRIVATE 
		device_dubhe_riscv
)

target_include_directories( ${PROJECT_NAME}.elf
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		inc
)

add_custom_target( ${PROJECT_NAME}.dump ALL
	COMMAND ${CMAKE_OBJDUMP} -dx ${PROJECT_NAME}.elf > ${PROJECT_NAME}.dump
	DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target( ${PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
	DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target( ${PROJECT_NAME}.hex ALL
	COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.bin > ${PROJECT_NAME}.hex
	DEPENDS ${PROJECT_NAME}.bin
)

set_property(TARGET ${PROJECT_NAME}.elf
	APPEND
	PROPERTY 
		ADDITIONAL_CLEAN_FILES
				${PROJECT_NAME}.map
				${PROJECT_NAME}.dump
				${PROJECT_NAME}.bin
				${PROJECT_NAME}.hex
)