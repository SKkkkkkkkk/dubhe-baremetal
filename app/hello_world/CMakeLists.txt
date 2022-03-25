cmake_minimum_required(VERSION 3.15)

set(SDK_DIR ${CMAKE_SOURCE_DIR}/../.. CACHE STRING "")
include (${SDK_DIR}/tools/cmake_toolchains/out_of_source.cmake)
set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/tools/cmake_toolchains/aarch64gcc.cmake)

set(PROJECT_NAME "a" CACHE STRING "Project name.")
set(DEVICE "A55" CACHE STRING "Device: A55, RISCY, CV32E40P.")
set(BOARD "RTL" CACHE STRING "Board: RTL, QEMU, FPGA, EVB")
set(BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug, FastRel, FastRelWithDebInfo, MinSizeRel, MinSizeRelWithDebInfo")

message(STATUS "Project     : ${PROJECT_NAME}")
message(STATUS "Device      : ${DEVICE}")
message(STATUS "Board       : ${BOARD}")
message(STATUS "Build Type  : ${BUILD_TYPE}")

set(compile_options_arch)
set(compile_options_warning -Wall -Werror)
set(compile_options_opt -ffunction-sections -fdata-sections)
set(link_options -Wl,--fatal-warning -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map)
set(device_lib)

if($CACHE{DEVICE} STREQUAL "A55") # "A55"
	set(compile_options_arch ${compile_options_arch} -mlittle-endian -march=armv8-a)
	set(link_options ${link_options} -T ${SDK_DIR}/devices/a55/src/aarch64.ld -nostartfiles --specs=nosys.specs)
	set(device_lib a55)
elseif($CACHE{DEVICE} STREQUAL "RISCY") # "RISCY"

elseif($CACHE{DEVICE} STREQUAL "CV32E40P") # "CV32E40P"

else()
	message(FATAL_ERROR "unsupport device!")
endif()

if($CACHE{BOARD} STREQUAL "RTL")
	set(compile_options_board -DRTL)
elseif($CACHE{BOARD} STREQUAL "QEMU")
	set(compile_options_board -DQEMU)
elseif($CACHE{BOARD} STREQUAL "FPGA")
	set(compile_options_board -DFPGA)
elseif($CACHE{BOARD} STREQUAL "EVB")
	set(compile_options_board -DEVB)
else()
	message(FATAL_ERROR "unsupport board!")
endif()


if($CACHE{BUILD_TYPE} STREQUAL "Debug")
	set(compile_options_opt ${compile_options_opt} -O0 -g3 -DDEBUG)
elseif($CACHE{BUILD_TYPE} STREQUAL "FastRel")
	set(compile_options_opt ${compile_options_opt} -Ofast -g0 -DFastRel)
elseif($CACHE{BUILD_TYPE} STREQUAL "FastRelWithDebInfo")
	set(compile_options_opt ${compile_options_opt} -Ofast -Og -g3 -DFastRelWithDebInfo)
elseif($CACHE{BUILD_TYPE} STREQUAL "MinSizeRel")
	set(compile_options_opt ${compile_options_opt} -Os -g0 -DMinSizeRel)
elseif($CACHE{BUILD_TYPE} STREQUAL "MinSizeRelWithDebInfo")
	set(compile_options_opt ${compile_options_opt} -Os -Og -g3 -DMinSizeRelWithDebInfo)
else()
	message(WARNING "unsupport BUILD_TYPE, using Debug!")
endif()

message(STATUS "compile_options_arch: ${compile_options_arch}")
message(STATUS "compile_options_board: ${compile_options_board}")
message(STATUS "compile_options_warning: ${compile_options_warning}")
message(STATUS "compile_options_opt: ${compile_options_opt}")
message(STATUS "link_options: ${link_options}")

project($CACHE{PROJECT_NAME}
  VERSION 0.1
  DESCRIPTION "Project: $CACHE{PROJECT_NAME}"
  LANGUAGES C ASM
)

add_executable(${PROJECT_NAME}.elf)

# add subdirectoy
if($CACHE{DEVICE} STREQUAL "A55") # "A55"
	add_subdirectory(${SDK_DIR}/devices/a55 a55.dir)
elseif($CACHE{DEVICE} STREQUAL "RISCY") # "RISCY"
	add_subdirectory(${SDK_DIR}/devices/riscy riscy.dir)
elseif($CACHE{DEVICE} STREQUAL "CV32E40P") # "CV32E40P"
	add_subdirectory(${SDK_DIR}/devices/cv32e40p cv32e40p.dir)
else()
	message(FATAL_ERROR "unsupport device!")
endif()

include (./app.cmake)
# app sources
target_sources(${PROJECT_NAME}.elf
	PRIVATE 
		${app_sources}
)

# app include dirs
target_include_directories(${PROJECT_NAME}.elf
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		${app_include_dirs}
)

# app compile options
target_compile_options(${PROJECT_NAME}.elf
	PRIVATE
		${compile_options_arch}
		${compile_options_warning}
		${compile_options_opt}
)

# link options
target_link_options(${PROJECT_NAME}.elf
	PRIVATE
		${link_options}
)

# link libs
target_link_libraries(${PROJECT_NAME}.elf
	PRIVATE 
		${device_lib}
		${extra_libs}
)

# link libs dirs
target_link_directories(${PROJECT_NAME}.elf
	PRIVATE
		${extra_libs_dirs}
)

# cutom targets
add_custom_target(${PROJECT_NAME}.dump ALL
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