# Works with 3.15 and tested through 3.21
cmake_minimum_required(VERSION 3.15...3.21)

# Project name and a few useful settings. Other commands can pick up the results
project(aarch64
  VERSION 0.1
  DESCRIPTION "aarch64 low level program project."
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

add_subdirectory(${SDK_DIR}/devices/dubhe_a55 dubhe_a55.dir)

# add_subdirectory(${SDK_DIR}/hal hal.dir)

add_executable(${PROJECT_NAME}.elf )

target_sources(${PROJECT_NAME}.elf
	PRIVATE 
		src/main.c
)

target_compile_options(${PROJECT_NAME}.elf
	PRIVATE
		-mlittle-endian -march=armv8-a #-mabi=ilp32
		-Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-pointer-to-int-cast -Wno-unused-but-set-variable
		-ffunction-sections -fdata-sections -nostdlib
		# -fshort-enums
		-fms-extensions
		$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
		$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
)

target_link_options( ${PROJECT_NAME}.elf
	PRIVATE
		-Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/${SDK_DIR}/devices/dubhe_a55/src/aarch64.ld
		-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
		-nostartfiles -nostdlib
		-Wl,--fatal-warnings -Wl,--gc-sections
)

target_link_libraries( ${PROJECT_NAME}.elf
	PRIVATE 
		dubhe_a55
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

set_property(TARGET ${PROJECT_NAME}.elf
	APPEND
	PROPERTY 
		ADDITIONAL_CLEAN_FILES
				${PROJECT_NAME}.map
				${PROJECT_NAME}.dump
				${PROJECT_NAME}.bin
)