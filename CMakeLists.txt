cmake_minimum_required(VERSION 3.15)
include (build_helper/out_of_source.cmake)
include (CMakeDependentOption)
set(SDK_DIR ${CMAKE_SOURCE_DIR})

# 选择project
set(PROJECT_NAME "a" CACHE STRING "Project name.")
set(PROJECT_PATH "app/a55/hello" CACHE STRING "Project path.")
if(IS_ABSOLUTE $CACHE{PROJECT_PATH})
	set(PROJECT_PATH "${PROJECT_PATH}")
else()
	set(PROJECT_PATH "$ENV{PWD}/${PROJECT_PATH}")
endif()

# 选择core
set(CORE "A55" CACHE STRING "Core: A55, M3, CV32E40P, RI5CY.")

# 选择borad
set(BOARD "QEMU" CACHE STRING "Board: RTL, QEMU, FPGA, EVB")

# 选择build type
set(BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug, FastRel, FastRelWithDebInfo, MinSizeRel, MinSizeRelWithDebInfo")

# 打印工程配置信息
message(STATUS "Project     : $CACHE{PROJECT_NAME}")
message(STATUS "Core        : $CACHE{CORE}")
message(STATUS "Board       : $CACHE{BOARD}")
message(STATUS "Build Type  : $CACHE{BUILD_TYPE}")
message(STATUS "Proj Path   : ${PROJECT_PATH}")

# 编译选项
## 通用的编译选项
set(compile_options_arch) # depends on core
set(compile_options_board) # depends on board
set(compile_options_opt)  # depends on build type
set(compile_options_common -Wall -Werror -ffunction-sections -fdata-sections)
set(link_options -static -Wl,--fatal-warning -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map -Wl,--print-memory-usage)
set(link_libs)
## 添加编译选项depends on CORE.
### A55
if($CACHE{CORE} STREQUAL "A55")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/aarch64gcc.cmake)
	set(compile_options_arch -mlittle-endian -march=armv8-a -DA55 -nostdlib)
	set(link_options ${link_options} -nostartfiles --specs=nosys.specs)
### M3
elseif($CACHE{CORE} STREQUAL "M3")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/armgcc.cmake)
	set(compile_options_arch ${compile_options_arch} -mlittle-endian -mcpu=cortex-m3 -mthumb -mabi=aapcs -DM3)
	set(link_options ${link_options} -nostartfiles --specs=nosys.specs --specs=nano.specs)
### CV32E40P
elseif($CACHE{CORE} STREQUAL "CV32E40P")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/riscv_gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -march=rv32imc -mabi=ilp32 -DCV32E40P)
	set(link_options ${link_options} -nostartfiles)
### RI5CY
elseif($CACHE{CORE} STREQUAL "RI5CY") 
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/ri5cy_gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -m32 -march=IMXpulpv2 -Wa,-march=IMXpulpv2 -DRI5CY)
	set(link_options ${link_options} -nostartfiles)
	set(link_options ${link_options} -L ${SDK_DIR}/hw/cores/riscv/ri5cy/ld -T ${SDK_DIR}/hw/cores/riscv/ri5cy/ld/idram.ld)
### unsupport
else()
	message(FATAL_ERROR "unsupport core!")
endif()

## 添加编译选项depends on BOARD.
### RTL
if($CACHE{BOARD} STREQUAL "RTL")
	set(compile_options_board -DRTL)
### QEMU
elseif($CACHE{BOARD} STREQUAL "QEMU")
	set(compile_options_board -DQEMU)
### FPGA
elseif($CACHE{BOARD} STREQUAL "FPGA")
	set(compile_options_board -DFPGA)
### EVB
elseif($CACHE{BOARD} STREQUAL "EVB")
	set(compile_options_board -DEVB)
### unsupport
else()
	message(FATAL_ERROR "unsupport board!")
endif()


## 添加编译选项depends on BUILD_TYPE.
### Debug
if($CACHE{BUILD_TYPE} STREQUAL "Debug")
	set(compile_options_opt ${compile_options_opt} -O0 -g3 -DDEBUG)
### FastRel
elseif($CACHE{BUILD_TYPE} STREQUAL "FastRel")
	set(compile_options_opt ${compile_options_opt} -Ofast -g0 -DFastRel)
### FastRelWithDebInfo
elseif($CACHE{BUILD_TYPE} STREQUAL "FastRelWithDebInfo")
	set(compile_options_opt ${compile_options_opt} -Ofast -Og -g3 -DFastRelWithDebInfo)
### MinSizeRel
elseif($CACHE{BUILD_TYPE} STREQUAL "MinSizeRel")
	set(compile_options_opt ${compile_options_opt} -Os -g0 -DMinSizeRel)
### MinSizeRelWithDebInfo
elseif($CACHE{BUILD_TYPE} STREQUAL "MinSizeRelWithDebInfo")
	set(compile_options_opt ${compile_options_opt} -Os -Og -g3 -DMinSizeRelWithDebInfo)
else()
### unsupport
	message(WARNING "unsupport BUILD_TYPE, using Debug!")
endif()



# 开始构建工程
project($CACHE{PROJECT_NAME}
  VERSION 1.0
  DESCRIPTION "dubhe project."
  LANGUAGES C ASM
)

## 获取工程配置信息(由app.cmake提供)
include (${PROJECT_PATH}/app.cmake)

add_executable($CACHE{PROJECT_NAME}.elf)

## 添加子模块
### 添加common通用模块
add_subdirectory(${SDK_DIR}/common)
set(link_libs ${link_libs} common)

### A55必须的模块
if($CACHE{CORE} STREQUAL "A55")
	add_subdirectory(${SDK_DIR}/hw/cores/arm/armv8/a/cortex-a55)
	set(link_libs ${link_libs} a55)
### M3必须的模块
elseif($CACHE{CORE} STREQUAL "M3")
	add_subdirectory(${SDK_DIR}/hw/cores/arm/armv7/m/coretex-m3)
	set(link_libs ${link_libs} m3)
### CV32E40P必须的模块
elseif($CACHE{CORE} STREQUAL "CV32E40P")
	add_subdirectory(${SDK_DIR}/hw/cores/riscv/cv32e40p)
	set(link_libs ${link_libs} cv32e40p)
### RI5CY必须的模块
elseif($CACHE{CORE} STREQUAL "RI5CY")
	add_subdirectory(${SDK_DIR}/hw/cores/riscv/ri5cy)
	set(link_libs ${link_libs} ri5cy)
### unsupport
else()
	message(FATAL_ERROR "unsupport core!")
endif()

### 添加可选模块
### USE_XLAT_TABLES_V2 (CORE==A55)
cmake_dependent_option(
    USE_XLAT_TABLES_V2 "Use xlat_tables_v2" ON
    "$CACHE{CORE} STREQUAL A55" OFF
)
if(USE_XLAT_TABLES_V2)
	add_subdirectory(${SDK_DIR}/libs/xlat_tables_v2)
	set(link_libs ${link_libs} xlat_tables_v2)
endif()

### USE_PL001 (BOARD==QEMU)
cmake_dependent_option(
    USE_PL001 "Use pl001" ON
    "$CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_PL001}" OFF
)
if(USE_PL001)
	add_subdirectory(${SDK_DIR}/hw/uart/pl001)
	set(link_libs ${link_libs} pl001)
endif()

### USE_DW_UART (BOARD!=QEMU)
cmake_dependent_option(
    USE_DW_UART "Use dw_apb_uart" ON
    "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_DW_APB_UART}" OFF
)
if(USE_DW_UART)
	add_subdirectory(${SDK_DIR}/hw/uart/dw_apb_uart)
	set(link_libs ${link_libs} dw_apb_uart)
endif()

### USE_DW_APB_TIMERS (BOARD!=QEMU)
cmake_dependent_option(
    USE_DW_APB_TIMERS "Use dw_apb_timers" ON
    "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_DW_APB_TIMERS}" OFF
)
if(USE_DW_APB_TIMERS)
	add_subdirectory(${SDK_DIR}/hw/timer/dw_apb_timers)
	set(link_libs ${link_libs} dw_apb_timers)
endif()

### USE_SYSTIMER (USE_ARCH_TIMER || USE_DW_APB_TIMERS)
cmake_dependent_option(
    USE_SYSTIMER "Use systimer" ON
    "USE_ARCH_TIMER OR USE_DW_APB_TIMERS" OFF
)
if(USE_DW_APB_TIMERS)
	add_subdirectory(${SDK_DIR}/libs/systimer)
	set(link_libs ${link_libs} systimer)
endif()

### USE_NEWLIB (USE_PL001 or USE_DW_UART)
cmake_dependent_option(
	USE_NEWLIB "Use newlib port" ON 
	"USE_PL001 OR USE_DW_UART" OFF
)
if(USE_NEWLIB)
	add_subdirectory(${SDK_DIR}/libs/newlib)
	set(link_libs ${link_libs} newlib_port)
endif()

### USE_SEEHI_PRINTF (USE_PL001 or USE_DW_UART)
cmake_dependent_option(
	USE_SEEHI_PRINT "Use seehi_printf" ON
	"USE_PL001 OR USE_DW_UART" OFF
)
if(USE_SEEHI_PRINT)
	add_subdirectory(${SDK_DIR}/libs/seehi_printf)
	set(link_libs ${link_libs} seehi_printf)
endif()

## 构建app
### app sources
target_sources($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${app_src}
)

### app include dirs
target_include_directories($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		${app_inc}
)

### app compile options
target_compile_options($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
)

### link options
target_link_options($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${compile_options_arch}
		${link_options}
)

### link libs
target_link_libraries($CACHE{PROJECT_NAME}.elf
	PRIVATE -Wl,--start-group ${link_libs} -Wl,--end-group linkerscript
)

# Cumtom targets
## dump
add_custom_target($CACHE{PROJECT_NAME}.dump ALL
	COMMAND ${CMAKE_OBJDUMP} -dx $CACHE{PROJECT_NAME}.elf > $CACHE{PROJECT_NAME}.dump
	DEPENDS $CACHE{PROJECT_NAME}.elf
    WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)

## bin
# add_custom_target( $CACHE{PROJECT_NAME}.bin ALL
# 	COMMAND ${CMAKE_OBJCOPY} -O binary $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.bin
# 	DEPENDS $CACHE{PROJECT_NAME}.elf
# 	WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
# )

# set_property(TARGET $CACHE{PROJECT_NAME}.elf
# 	APPEND
# 	PROPERTY
# 		ADDITIONAL_CLEAN_FILES
# 				$CACHE{PROJECT_NAME}.bin
# 				$CACHE{PROJECT_NAME}.dump
# )

## bin
if(${CORE} STREQUAL "A55")
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
endif()