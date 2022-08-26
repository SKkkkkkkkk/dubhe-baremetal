cmake_minimum_required(VERSION 3.15)
include (build_helper/out_of_source.cmake)
include (CMakeDependentOption)
set(SDK_DIR ${CMAKE_SOURCE_DIR})

# 选择project
set(PROJECT_NAME "a" CACHE STRING "Project name.")
set(PROJECT_PATH "" CACHE STRING "Project path.")
set(EXTRA_SRC "" CACHE STRING "a extra src.")
if(IS_ABSOLUTE $CACHE{PROJECT_PATH})
	set(PROJECT_PATH "${PROJECT_PATH}")
else()
	set(PROJECT_PATH "$ENV{PWD}/${PROJECT_PATH}")
endif()
if(IS_ABSOLUTE ${EXTRA_SRC})
	set(EXTRA_SRC "${EXTRA_SRC}")
else()
	set(EXTRA_SRC "$ENV{PWD}/${EXTRA_SRC}")
endif()

# 选择core
set(CORE "A55" CACHE STRING "Core: A55, M3, CV32E40P.")

# 选择borad
set(BOARD "QEMU" CACHE STRING "Board: RTL, QEMU, FPGA, EVB")

# 选择build type
set(BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug, FastRel, FastRelWithDebInfo, MinSizeRel, MinSizeRelWithDebInfo")

# Where to load & run
set(WHERE_TO_LOAD "ROM_AND_RAM" CACHE STRING "Where to load & run: ROM_AND_RAM, ALL_IN_RAM, ALL_IN_DDR")


# 打印工程配置信息
message(STATUS "Project     : $CACHE{PROJECT_NAME}")
message(STATUS "Core        : $CACHE{CORE}")
message(STATUS "Board       : $CACHE{BOARD}")
message(STATUS "Build Type  : $CACHE{BUILD_TYPE}")
if(NOT $CACHE{PROJECT_PATH} STREQUAL "")
	message(STATUS "Proj Path   : ${PROJECT_PATH}")
else()
	message(STATUS "Proj Path   : No Proj Path!")
endif()
if(NOT $CACHE{EXTRA_SRC} STREQUAL "")
	message(STATUS "Extra SRC   : ${EXTRA_SRC}")
else()
	message(STATUS "Extra SRC   : No Extra SRC.")
endif()
if($CACHE{CORE} STREQUAL "A55")
	message(STATUS "Where to load & run: ${WHERE_TO_LOAD}")
endif()

# 编译选项
## 通用的编译选项
set(compile_options_arch) # depends on core
set(compile_options_board) # depends on board
set(compile_options_opt)  # depends on build type --specs=nosys.specs
set(compile_options_common -Wall -Werror -ffunction-sections -fdata-sections )
set(link_options -static -nostartfiles --specs=nosys.specs -Wl,--fatal-warning -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map -Wl,--print-memory-usage)
set(link_libs)
## 添加编译选项depends on CORE.
### A55
if($CACHE{CORE} STREQUAL "A55")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/aarch64gcc.cmake)
	set(compile_options_arch -mlittle-endian -march=armv8.1-a -mcmodel=large -DA55 -nostdlib)
	set(link_options ${link_options})
### M3
elseif($CACHE{CORE} STREQUAL "M3")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/armgcc.cmake)
	set(compile_options_arch ${compile_options_arch} -mlittle-endian -mcpu=cortex-m3 -mthumb -mabi=aapcs -DM3)
	set(link_options ${link_options} --specs=nano.specs)
### CV32E40P
elseif($CACHE{CORE} STREQUAL "CV32E40P")
	set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/riscv_gcc.cmake)
	set(compile_options_arch ${compile_options_arch} -march=rv32imc -mabi=ilp32 -DCV32E40P)
	set(link_options ${link_options})
### unsupport
else()
	message(FATAL_ERROR "unsupport core!")
endif()

if(NOT $CACHE{CORE} STREQUAL "M3")
	set(compile_options_align -mstrict-align)
else()
	set(compile_options_align -mno-unaligned-access)
endif()

## 添加编译选项depends on BOARD.
### RTL
if($CACHE{BOARD} STREQUAL "RTL")
	set(compile_options_board -DRTL -DWHERE_TO_LOAD=$CACHE{WHERE_TO_LOAD})
### QEMU
elseif($CACHE{BOARD} STREQUAL "QEMU")
	set(compile_options_board -DQEMU -DWHERE_TO_LOAD=$CACHE{WHERE_TO_LOAD})
### FPGA
elseif($CACHE{BOARD} STREQUAL "FPGA")
	set(compile_options_board -DFPGA -DWHERE_TO_LOAD=$CACHE{WHERE_TO_LOAD})
### EVB
elseif($CACHE{BOARD} STREQUAL "EVB")
	set(compile_options_board -DEVB -DWHERE_TO_LOAD=$CACHE{WHERE_TO_LOAD})
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
set(ENABLE_USE_PL001 OFF)
set(ENABLE_USE_DW_APB_UART OFF)
set(ENABLE_USE_DW_APB_TIMERS OFF)

## 获取工程配置信息(由app.cmake提供)
if(NOT $CACHE{PROJECT_PATH} STREQUAL "")
	include (${PROJECT_PATH}/app.cmake)
endif()

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
### unsupport
else()
	message(FATAL_ERROR "unsupport core!")
endif()

### 添加可选模块
if(NOT $CACHE{CORE} STREQUAL CV32E40P)
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
		"USE_DW_APB_TIMERS" OFF
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
	# cmake_dependent_option(
	# 	USE_SEEHI_PRINT "Use seehi_printf" ON
	# 	"USE_PL001 OR USE_DW_UART" OFF
	# )
	# if(USE_SEEHI_PRINT)
	# 	add_subdirectory(${SDK_DIR}/libs/seehi_printf)
	# 	set(link_libs ${link_libs} seehi_printf)
	# endif()
endif()

## 构建app
### app sources
target_sources($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${app_src}
		${EXTRA_SRC}
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
add_custom_target( $CACHE{PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} -O binary $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.bin
	COMMAND ${CMAKE_OBJCOPY} -O verilog  $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.verilog.hex
	DEPENDS $CACHE{PROJECT_NAME}.elf
	WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)

set_property(TARGET $CACHE{PROJECT_NAME}.elf
	APPEND
	PROPERTY
		ADDITIONAL_CLEAN_FILES
				$CACHE{PROJECT_NAME}.bin
				$CACHE{PROJECT_NAME}.dump
				$CACHE{PROJECT_NAME}.verilog.hex
)