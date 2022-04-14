cmake_minimum_required(VERSION 3.15)

set(SDK_DIR ${CMAKE_SOURCE_DIR})
include (${SDK_DIR}/tools/cmake_toolchains/out_of_source.cmake)


set(PROJECT_NAME "a" CACHE STRING "Project name.")
set(PROJECT_PATH "" CACHE STRING "Project path.(not ABS)")
set(EXTRA_SRC "" CACHE STRING "a extra src.") #向下兼容
set(DEVICE "A55" CACHE STRING "Device: A55, RI5CY, CV32E40P, M3.")
set(BOARD "RTL" CACHE STRING "Board: RTL, QEMU, FPGA, EVB")
set(BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug, FastRel, FastRelWithDebInfo, MinSizeRel, MinSizeRelWithDebInfo")


if(IS_ABSOLUTE ${PROJECT_PATH})
	set(ABS_PROJECT_PATH "${PROJECT_PATH}" CACHE STRING "ABS Project path.")
else()
	set(ABS_PROJECT_PATH "$ENV{PWD}/${PROJECT_PATH}" CACHE STRING "ABS Project path.")
endif()

if(IS_ABSOLUTE ${EXTRA_SRC})
	set(ABS_EXTRA_SRC "${EXTRA_SRC}" CACHE STRING "ABS EXTRA_SRC.")
else()
	set(ABS_EXTRA_SRC "$ENV{PWD}/${EXTRA_SRC}" CACHE STRING "ABS EXTRA_SRC.")
endif()



message(STATUS "Project     : ${PROJECT_NAME}")
message(STATUS "Device      : ${DEVICE}")
message(STATUS "Board       : ${BOARD}")
message(STATUS "Build Type  : ${BUILD_TYPE}")
if(NOT $CACHE{PROJECT_PATH} STREQUAL "")
message(STATUS "Proj Path   : ${ABS_PROJECT_PATH}")
else()
message(STATUS "No PROJECT_PATH.")
endif()
if(NOT $CACHE{EXTRA_SRC} STREQUAL "")
message(STATUS "EXTRA_SRC   : ${ABS_EXTRA_SRC}")
else()
message(STATUS "No EXTRA_SRC.")
endif()

set(compile_options_arch)
set(compile_options_warning -Wall -Werror)
set(compile_options_opt -ffunction-sections -fdata-sections)
set(link_options -Wl,--fatal-warning -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map)
set(device_lib)

if($CACHE{DEVICE} STREQUAL "A55") # "A55"
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/tools/cmake_toolchains/aarch64gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -mlittle-endian -march=armv8-a -DA55)
	set(link_options ${link_options} -static -nostartfiles --specs=nosys.specs)
	if($CACHE{BOARD} STREQUAL "QEMU")
		set(link_options ${link_options} -T ${SDK_DIR}/devices/a55/arch/aarch64/src/aarch64_qemu.ld)
	else()
		set(link_options ${link_options} -T ${SDK_DIR}/devices/a55/arch/aarch64/src/aarch64.ld)
	endif()
	set(device_lib a55)
elseif($CACHE{DEVICE} STREQUAL "RI5CY") # "RI5CY"
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/tools/cmake_toolchains/riscv_gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -m32 -march=IMXpulpv2 -Wa,-march=IMXpulpv2 -DRI5CY)
	set(link_options ${link_options} -L ${SDK_DIR}/devices/ri5cy/ld -T ${SDK_DIR}/devices/ri5cy/ld/idram.ld -static -nostartfiles)
	set(device_lib ri5cy)
elseif($CACHE{DEVICE} STREQUAL "CV32E40P") # "CV32E40P"
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/tools/cmake_toolchains/riscv_gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -march=rv32imc -mabi=ilp32 -DCV32E40P)
	set(link_options ${link_options} -T ${SDK_DIR}/devices/cv32e40p/link.ld -static -nostartfiles)
	set(device_lib cv32e40p)
elseif($CACHE{DEVICE} STREQUAL "M3") # "M3"
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/tools/cmake_toolchains/armgcc.cmake)
	set(compile_options_arch ${compile_options_arch} -mlittle-endian -mcpu=cortex-m3 -mthumb -mabi=aapcs -DM3)
	set(link_options ${link_options} -static -nostartfiles --specs=nosys.specs)
	if($CACHE{BOARD} STREQUAL "QEMU")
		# set(link_options ${link_options} -T ${SDK_DIR}/devices/m3/src/m3.ld)
	else()
		set(link_options ${link_options} -T ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.ld)
	endif()
	set(device_lib m3)
	set(CPP arm-none-eabi-cpp)
	set(lds ${SDK_DIR}/devices/m3/src/m3.ld.S)
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

# message(STATUS "compile_options_arch: ${compile_options_arch}")
# message(STATUS "compile_options_board: ${compile_options_board}")
# message(STATUS "compile_options_warning: ${compile_options_warning}")
# message(STATUS "compile_options_opt: ${compile_options_opt}")
# message(STATUS "link_options: ${link_options}")

project($CACHE{PROJECT_NAME}
  VERSION 0.1
  DESCRIPTION "Project: $CACHE{PROJECT_NAME}"
  LANGUAGES C ASM
)

add_executable(${PROJECT_NAME}.elf)

# add subdirectoy
if($CACHE{DEVICE} STREQUAL "A55") # "A55"
	add_subdirectory(${SDK_DIR}/devices/a55 a55.dir)
elseif($CACHE{DEVICE} STREQUAL "RI5CY") # "RI5CY"
	add_subdirectory(${SDK_DIR}/devices/ri5cy ri5cy.dir)
elseif($CACHE{DEVICE} STREQUAL "CV32E40P") # "CV32E40P"
	add_subdirectory(${SDK_DIR}/devices/cv32e40p cv32e40p.dir)
elseif($CACHE{DEVICE} STREQUAL "M3") # "M3"
	add_subdirectory(${SDK_DIR}/devices/m3 m3.dir)
else()
	message(FATAL_ERROR "unsupport device!")
endif()

file(GLOB_RECURSE C_SRCS ${SDK_DIR}/sources/common/*.c)
file(GLOB_RECURSE S_SRCS ${SDK_DIR}/sources/common/*.S)
set(SRCS ${S_SRCS} ${C_SRCS} $CACHE{ABS_EXTRA_SRC})
if(NOT $CACHE{PROJECT_PATH} STREQUAL "")
include (${ABS_PROJECT_PATH}/app.cmake)
endif()
# app sources
target_sources(${PROJECT_NAME}.elf
	PRIVATE
		${app_sources}
		${SRCS}
)

# app include dirs
target_include_directories(${PROJECT_NAME}.elf
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		include
		${app_include_dirs}
		${EXTRA_INC_DIR}
)

# app compile options
target_compile_options(${PROJECT_NAME}.elf
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
)

# link options
target_link_options(${PROJECT_NAME}.elf
	PRIVATE
		${compile_options_arch}
		${link_options}
)
# ld.S => ld
# add_dependencies(${PROJECT_NAME}.elf ${PROJECT_NAME}.ld)
# set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.ld)
# add_custom_target(${PROJECT_NAME}.ld
# 	COMMAND ${CPP} -P -x assembler-with-cpp -D__LINKER__ -o ${PROJECT_NAME}.ld ${lds}
# 	DEPENDS
# 		${lds}
# )
if($CACHE{DEVICE} STREQUAL "M3") 
	set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.ld)
	add_dependencies(${PROJECT_NAME}.elf lds2ld)
	add_custom_target(lds2ld
		DEPENDS ${PROJECT_NAME}.ld
	)
	add_custom_command(
		OUTPUT ${PROJECT_NAME}.ld
		COMMAND ${CPP} -P -x assembler-with-cpp -D__LINKER__ -o ${PROJECT_NAME}.ld ${lds}
		DEPENDS ${lds}
		VERBATIM
	)
endif()

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

# if(${BOARD} STREQUAL "RTL")
	if(${DEVICE} STREQUAL "A55")
		add_custom_target( ${PROJECT_NAME}.bin ALL
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
			COMMAND ${CMAKE_OBJCOPY} -O binary -j .text -j .ARM.extab -j .ARM.exidx -j .rodata ${PROJECT_NAME}.elf ${PROJECT_NAME}.ro.bin
			COMMAND ${CMAKE_OBJCOPY} -O binary -j .data ${PROJECT_NAME}.elf ${PROJECT_NAME}.rw.bin
			DEPENDS ${PROJECT_NAME}.elf
		)
		add_custom_target( ${PROJECT_NAME}.hex ALL
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.bin > ${PROJECT_NAME}.hex
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.ro.bin > ${PROJECT_NAME}.ro.hex
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.rw.bin > ${PROJECT_NAME}.rw.hex
			DEPENDS ${PROJECT_NAME}.bin
		)
		set_property(TARGET ${PROJECT_NAME}.elf
			APPEND
			PROPERTY 
				ADDITIONAL_CLEAN_FILES
						${PROJECT_NAME}.bin
						${PROJECT_NAME}.ro.bin
						${PROJECT_NAME}.rw.bin
						${PROJECT_NAME}.hex
						${PROJECT_NAME}.ro.hex
						${PROJECT_NAME}.rw.hex
		)
	elseif(${DEVICE} STREQUAL "RI5CY")
		add_custom_target( ${PROJECT_NAME}.bin ALL
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
			COMMAND ${CMAKE_OBJCOPY} -O binary -j .text ${PROJECT_NAME}.elf ${PROJECT_NAME}.ro.bin
			COMMAND ${CMAKE_OBJCOPY} -O binary -j .data ${PROJECT_NAME}.elf ${PROJECT_NAME}.rw.bin
			DEPENDS ${PROJECT_NAME}.elf
		)
		add_custom_target( ${PROJECT_NAME}.hex ALL
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.bin > ${PROJECT_NAME}.hex
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.ro.bin > ${PROJECT_NAME}.ro.hex
			COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.rw.bin > ${PROJECT_NAME}.rw.hex
			DEPENDS
				${PROJECT_NAME}.bin
		)
		set_property(TARGET ${PROJECT_NAME}.elf
			APPEND
			PROPERTY 
				ADDITIONAL_CLEAN_FILES
						${PROJECT_NAME}.bin
						${PROJECT_NAME}.ro.bin
						${PROJECT_NAME}.rw.bin
						${PROJECT_NAME}.hex
						${PROJECT_NAME}.ro.hex
						${PROJECT_NAME}.rw.hex
		)
	elseif(${DEVICE} STREQUAL "CV32E40P")
		add_custom_target( ${PROJECT_NAME}.bin ALL
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
			DEPENDS ${PROJECT_NAME}.elf
		)
		add_custom_target( ${PROJECT_NAME}.hex ALL
			COMMAND ${CMAKE_OBJCOPY} -O verilog  ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
			DEPENDS ${PROJECT_NAME}.elf
		)
		set_property(TARGET ${PROJECT_NAME}.elf
			APPEND
			PROPERTY 
				ADDITIONAL_CLEAN_FILES
						${PROJECT_NAME}.bin
						${PROJECT_NAME}.hex
		)
	elseif(${DEVICE} STREQUAL "M3")
		add_custom_target( ${PROJECT_NAME}.bin ALL
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
			DEPENDS ${PROJECT_NAME}.elf
		)
		add_custom_target( ${PROJECT_NAME}.hex ALL
			COMMAND ${CMAKE_OBJCOPY} -O verilog  ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
			DEPENDS ${PROJECT_NAME}.elf
		)
		set_property(TARGET ${PROJECT_NAME}.elf
			APPEND
			PROPERTY 
				ADDITIONAL_CLEAN_FILES
						${PROJECT_NAME}.bin
						${PROJECT_NAME}.hex
		)
	else()
		message(FATAL_ERROR "unsupport device!")
	endif()
# endif()

set_property(TARGET ${PROJECT_NAME}.elf
	APPEND
	PROPERTY 
		ADDITIONAL_CLEAN_FILES
				${PROJECT_NAME}.map
				${PROJECT_NAME}.dump
)