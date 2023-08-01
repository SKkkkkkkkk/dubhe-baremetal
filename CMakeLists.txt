cmake_minimum_required(VERSION 3.15...3.26)
include (build_helper/out_of_source.cmake)
include (CMakeDependentOption)
set(SDK_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# 选择Hardware Version

set(HW_VERSION "HW_VERSION_MPW" CACHE STRING "Hardware Version: HW_VERSION_MPW, HW_VERSION_FM.")

# 选择Primary Core

set(PRIMARY_CORE "0" CACHE STRING "Primary Core: 0, 1, 2, 3.")

# 选择borad

set(BOARD "QEMU" CACHE STRING "Board: RTL, QEMU, FPGA, EVB")

# 选择OS

set(OS "NO_SYS" CACHE STRING "OS: NO_SYS, FreeRTOS")

# 选择build type

set(BUILD_TYPE "Debug" CACHE STRING "Build Type: Debug, FastRel, FastRelWithDebInfo, MinSizeRel, MinSizeRelWithDebInfo")

# Where to load & run

if($CACHE{CORE} STREQUAL "A55")
    set(WHERE_TO_LOAD "ROM" CACHE STRING "Where to load & run: ROM, ALL_IN_SYSRAM, ALL_IN_DDR")
elseif($CACHE{CORE} STREQUAL "M3")
    set(WHERE_TO_LOAD "ROM" CACHE STRING "Where to load & run: ROM, ALL_IN_AXI_SRAM, ALL_IN_SYSRAM, ALL_IN_DDR, MEMMAP_SCHEME_1")
elseif($CACHE{CORE} STREQUAL "CV32E40P")
    set(WHERE_TO_LOAD "UNSUPPORT" CACHE STRING "Unsupport in CV32E40P")
else()
    message(FATAL_ERROR "unsupport core!")
endif()

# Enable MMU&Cache
cmake_dependent_option(
    ENABLE_MMU_CACHE "Enable MMU&Cache: ON, OFF" OFF
    "$CACHE{CORE} STREQUAL A55" OFF
)

# GIT_HASH
## 在程序中可以打印git 提交版本hash值。printf("git hash:"GIT_HASH)
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE git_hash
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Build Timestamp
string(TIMESTAMP BUILD_TIMESTAMP "%Y.%m.%d %H:%M:%S")
message(STATUS ${BUILD_TIMESTAMP} )

# 打印工程配置信息

message(STATUS "Project     : $CACHE{PROJECT_NAME}")
message(STATUS "HW_VERSION  : $CACHE{HW_VERSION}")
message(STATUS "Core        : $CACHE{CORE}")
message(STATUS "OS          : $CACHE{OS}")
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
if($CACHE{CORE} STREQUAL "A55" OR $CACHE{CORE} STREQUAL "M3")
    message(STATUS "Where to load & run: ${WHERE_TO_LOAD}")
endif()

# 编译选项

## 通用的编译选项

set(compile_options_arch) # depends on core
set(compile_options_board) # depends on board
set(compile_options_opt)  # depends on build type --specs=nosys.specs
set(compile_options_common -Wall -Werror -ffunction-sections -fdata-sections -DGIT_HASH="${git_hash}" -DBUILD_TIMESTAMP="${BUILD_TIMESTAMP}")
set(link_options -static -nostartfiles -Wl,--fatal-warning -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map -Wl,--print-memory-usage)
set(link_libs)

set(compile_options_cxx -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit -fno-threadsafe-statics)

## 添加编译选项depends on CORE.

### A55

if($CACHE{CORE} STREQUAL "A55")
    set(STDLIB_SRC "newlib" CACHE STRING "STDLIB_SRC: newlib, aarch64_libc.")
    set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/aarch64gcc.cmake)
    set(compile_options_arch -mlittle-endian -march=armv8.1-a -DA55)

    set(compile_options_common ${compile_options_common} -DPRIMARY_CORE=$CACHE{PRIMARY_CORE})

    if($CACHE{STDLIB_SRC} STREQUAL "aarch64_libc")
        set(compile_options_common ${compile_options_common} -Daarch64_libc -nostdinc -isystem${SDK_DIR}/libs/aarch64_libc/inc -isystem${SDK_DIR}/libs/aarch64_libc/inc/aarch64)
        set(link_options ${link_options} -nostdlib)
    else()
        set(link_options ${link_options} -Wl,--wrap=memcpy -Wl,--wrap=memmove -Wl,--wrap=memset -Wl,--wrap=memcmp)
    endif()

    if($CACHE{ENABLE_MMU_CACHE})
        set(compile_options_common ${compile_options_common} -DENABLE_MMU_CACHE=1)
    endif()

### M3

elseif($CACHE{CORE} STREQUAL "M3")
    set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/aarch32gcc.cmake)
    set(compile_options_arch ${compile_options_arch} -mlittle-endian -mcpu=cortex-m3 -mthumb -mabi=aapcs -DM3)
    set(link_options ${link_options} --specs=nano.specs --specs=nosys.specs)

### CV32E40P

elseif($CACHE{CORE} STREQUAL "CV32E40P")
    set(CMAKE_TOOLCHAIN_FILE ${SDK_DIR}/build_helper/riscv_gcc.cmake)
    set(compile_options_arch ${compile_options_arch} -march=rv32im_zicsr -mabi=ilp32 -DCV32E40P)

### unsupport

else()
    message(FATAL_ERROR "unsupport core!")
endif()

## 编译器对齐在arm-none-eabi-gcc编译器中不太一样

if(NOT $CACHE{CORE} STREQUAL "M3")
    set(compile_options_align -mstrict-align)
else()
    set(compile_options_align -mno-unaligned-access)
endif()

## 添加编译选项depends on HW_VERSION.

if($CACHE{HW_VERSION} STREQUAL "HW_VERSION_MPW")
    set(compile_options_common ${compile_options_common} -DHW_VERSION_MPW)
elseif($CACHE{HW_VERSION} STREQUAL "HW_VERSION_FM")
    set(compile_options_common ${compile_options_common} -DHW_VERSION_FM)
else()
    message(FATAL_ERROR "Unsupport HW_VERSION!")
endif()

## 添加编译选项depends on OS.
if($CACHE{OS} STREQUAL "NO_SYS")
    set(compile_options_common ${compile_options_common} -DNO_SYS)
elseif($CACHE{OS} STREQUAL "FreeRTOS")
    set(compile_options_common ${compile_options_common} -DFREERTOS)
else()
    message(FATAL_ERROR "Unsupport OS platform!")
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
  LANGUAGES C CXX ASM
)

## With ld.bfd version 2.39 and newer new warnings are added. Skip those since we are not loaded by a elf loader.
if(${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL 12.2.0)
    set(link_options ${link_options} -Wl,--no-warn-rwx-segments)
endif()

## 每个模块添加一个使能开关，ON的一般是最小系统必须的，凡是不是必须的都默认OFF，如app工程需要，在app.cmake中再开启

### HW

set(ENABLE_USE_PL011 ON)
set(ENABLE_USE_UART_16550 OFF) # UART_16550驱动用作DW_APB_UART驱动，可能导致波特率精度不够
set(ENABLE_USE_DW_APB_UART ON)
set(ENABLE_USE_DW_APB_TIMERS ON)
set(ENABLE_USE_DW_APB_GPIO ON)
set(ENABLE_USE_DW_APB_SSI OFF)
set(ENABLE_USE_PM OFF)
set(ENABLE_USE_I2C OFF)
set(ENABLE_USE_PINMUX ON)
set(ENABLE_USE_WDT OFF)
set(ENABLE_USE_PMU OFF)
set(ENABLE_USE_I2C_WO OFF)
set(ENABLE_USE_MAILBOX OFF)
set(ENABLE_USE_DDR ON)
set(ENABLE_USE_SYSCTRL ON)
set(ENABLE_USE_PMON0 OFF)


### LIBS

set(ENABLE_USE_SYSTIMER ON)
set(ENABLE_USE_NOR_FLASH OFF)
set(ENABLE_USE_PMIC OFF)
set(ENABLE_USE_tinyCLI OFF)
set(ENABLE_USE_SEMIHOSTING OFF)
set(ENABLE_USE_CRC OFF)
set(ENABLE_USE_xyzModem OFF)
set(ENABLE_USE_IMAGE_STATES OFF)

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

    if($CACHE{STDLIB_SRC} STREQUAL "aarch64_libc")
        add_subdirectory(${SDK_DIR}/libs/aarch64_libc aarch64_libc)
        set(link_libs ${link_libs} aarch64_libc)
    elseif($CACHE{STDLIB_SRC} STREQUAL "newlib")
        add_subdirectory(${SDK_DIR}/libs/newlib newlib_port)
        set(link_libs ${link_libs} newlib_port)
    endif()

### M3必须的模块

elseif($CACHE{CORE} STREQUAL "M3")
    add_subdirectory(${SDK_DIR}/hw/cores/arm/armv7/m/coretex-m3)
    add_subdirectory(${SDK_DIR}/libs/newlib newlib_port)
    set(link_libs ${link_libs} m3 newlib_port)

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

    ### USE_PL011 (BOARD==QEMU)
    cmake_dependent_option(
        USE_PL011 "Use pl011" ON
        "$CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_PL011}" OFF
    )
    if(USE_PL011)
        add_subdirectory(${SDK_DIR}/hw/uart/pl011)
        set(link_libs ${link_libs} pl011)
    endif()

    ### USE_UART_16550 (BOARD!=QEMU)
    cmake_dependent_option(
        USE_UART_16550 "Use 16550" ON
        "$CACHE{CORE} STREQUAL A55;NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_UART_16550}" OFF
    )
    if(USE_UART_16550)
        add_subdirectory(${SDK_DIR}/hw/uart/uart_16550)
        set(link_libs ${link_libs} uart_16550)
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

    ### USE_DW_APB_SSI (BOARD!=QEMU)
    cmake_dependent_option(
        USE_DW_APB_SSI "Use dw_apb_ssi" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_DW_APB_SSI}" OFF
    )
    if(USE_DW_APB_SSI)
        add_subdirectory(${SDK_DIR}/hw/spi/dw_apb_ssi)
        set(link_libs ${link_libs} dw_apb_ssi)
    endif()

    ### USE_DW_APB_GPIO (BOARD!=QEMU)
    cmake_dependent_option(
        USE_DW_APB_GPIO "Use dw_apb_gpio" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_DW_APB_GPIO}" OFF
    )
    if(USE_DW_APB_GPIO)
        add_subdirectory(${SDK_DIR}/hw/gpio/dw_apb_gpio)
        set(link_libs ${link_libs} dw_apb_gpio)
    endif()

    cmake_dependent_option(
        USE_DW_APB_GPIO "Use i2c" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_I2C}" OFF
    )
    if(ENABLE_USE_I2C)
        add_subdirectory(${SDK_DIR}/hw/i2c)
        set(link_libs ${link_libs} i2c)
    endif()


    ### USE_SYSTIMER (USE_ARCH_TIMER || USE_DW_APB_TIMERS)
    cmake_dependent_option(
        USE_SYSTIMER "Use systimer" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;USE_DW_APB_TIMERS;${ENABLE_USE_SYSTIMER}" OFF
    )
    if(USE_SYSTIMER)
        add_subdirectory(${SDK_DIR}/libs/systimer)
        set(link_libs ${link_libs} systimer)
    endif()

    ### USE_NOR_FLASH (USE_DW_APB_SSI && ENABLE_USE_NOR_FLASH)
    cmake_dependent_option(
        USE_NOR_FLASH "Use nor flash" ON
        "USE_DW_APB_SSI;ENABLE_USE_NOR_FLASH" OFF
    )
    if(USE_NOR_FLASH)
        add_subdirectory(${SDK_DIR}/libs/flash/nor)
        set(link_libs ${link_libs} nor_flash)
    endif()

    ### OS
    if($CACHE{OS} STREQUAL "NO_SYS")
    elseif($CACHE{OS} STREQUAL "FreeRTOS")
        add_subdirectory(${SDK_DIR}/libs/rtos/freertos/kernel/v10_5_1)
        set(link_libs ${link_libs} freertos_kernel)
    else()
        message(FATAL_ERROR "Unsupport OS platform!")
    endif()

    ### USE_PM
    cmake_dependent_option(
        USE_PM "Use Power Management" ON
        "ENABLE_USE_PM;$CACHE{CORE} STREQUAL M3" OFF
    )
    if(USE_PM)
        set(compile_options_board ${compile_options_board} -DCONFIG_PM)
        add_subdirectory(${SDK_DIR}/hw/pm)
        set(link_libs ${link_libs} pm)
    endif()

    ### USE_PMIC
    cmake_dependent_option(
		USE_PMIC "Use PMIC" ON
        "ENABLE_USE_PMIC" OFF
    )
    if(USE_PMIC)
        add_subdirectory(${SDK_DIR}/libs/pmic/axp2101)
        set(link_libs ${link_libs} axp2101)
    endif()

     ### USE_tinyCLI
    cmake_dependent_option(
        USE_tinyCLI "Use tinyCLI" ON
        "ENABLE_USE_tinyCLI;" OFF
    )
    if(USE_tinyCLI)
        add_subdirectory(${SDK_DIR}/libs/tinyCLI)
        set(link_libs ${link_libs} tinyCLI)
    endif()

    ### USE_SEMIHOSTING
    cmake_dependent_option(
        USE_SEMIHOSTING "Use semihosting" ON
        "ENABLE_USE_SEMIHOSTING;" OFF
    )
    if(USE_SEMIHOSTING)
        add_subdirectory(${SDK_DIR}/libs/semihosting)
        set(link_libs ${link_libs} semihosting)
    endif()

    ### USE_CRC
    cmake_dependent_option(
        USE_CRC "Use crc" ON
        "ENABLE_USE_CRC" OFF
    )
    if(USE_CRC)
        add_subdirectory(${SDK_DIR}/libs/crc)
        set(link_libs ${link_libs} crc)
    endif()

    ### USE_xyzModem
    cmake_dependent_option(
        USE_xyzModem "Use xyzModem" ON
        "ENABLE_USE_xyzModem;USE_CRC" OFF
    )
    if(USE_xyzModem)
        add_subdirectory(${SDK_DIR}/libs/xyzModem)
        set(link_libs ${link_libs} xyzModem)
    endif()

    ### USE_IMAGE_STATES
    cmake_dependent_option(
        USE_IMAGE_STATES "Use image_states" ON
        "ENABLE_USE_IMAGE_STATES" OFF
    )
    if(USE_IMAGE_STATES)
        add_subdirectory(${SDK_DIR}/libs/image_states)
        set(link_libs ${link_libs} image_states)
    endif()

    ### USE_PINMUX
    cmake_dependent_option(
        USE_PINMUX "Use pinmux" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;ENABLE_USE_PINMUX;" OFF
    )
    if(USE_PINMUX)
        add_subdirectory(${SDK_DIR}/hw/pinmux)
        set(link_libs ${link_libs} pinmux)
    endif()

    ### USE_WDT
    cmake_dependent_option(
        USE_WDT "Use dw_apb_wdt" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;ENABLE_USE_WDT" OFF
    )
    if(USE_WDT)
        add_subdirectory(${SDK_DIR}/hw/wdt/dw_apb_wdt)
        set(link_libs ${link_libs} dw_apb_wdt)
    endif()

    ### USE_PMU
    cmake_dependent_option(
        USE_PMU "Use seehi pmu" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;ENABLE_USE_PMU" OFF
    )
    if(USE_PMU)
        add_subdirectory(${SDK_DIR}/hw/pmu)
        set(link_libs ${link_libs} pmu)
    endif()

	### USE_I2C_WO
    cmake_dependent_option(
        USE_I2C_WO "Use seehi i2c wo" ON
		"NOT $CACHE{BOARD} STREQUAL QEMU;ENABLE_USE_I2C_WO" OFF
    )
    if(USE_I2C_WO)
        add_subdirectory(${SDK_DIR}/hw/i2c_wo)
        set(link_libs ${link_libs} i2c_wo)
    endif()

    ### USE_MAILBOX (BOARD!=QEMU)
    cmake_dependent_option(
        USE_MAILBOX "Use Mailbox" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_MAILBOX}" OFF
    )
    if(USE_MAILBOX)
        add_subdirectory(${SDK_DIR}/hw/mailbox)
        set(link_libs ${link_libs} mailbox)
    endif()


    ### USE_DDR (BOARD!=QEMU)
    cmake_dependent_option(
        USE_DDR "Use ddr_driver to init ddr." ON
        "$CACHE{BOARD} STREQUAL EVB;${ENABLE_USE_DDR}" OFF
    )
    if(USE_DDR)
        add_subdirectory(${SDK_DIR}/hw/ddr)
        set(link_libs ${link_libs} ddr)
    endif()

    ### USE_SYSCTRL
    cmake_dependent_option(
        USE_SYSCTRL "Use sysctrl." ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_SYSCTRL}" OFF
    )
    if(USE_SYSCTRL)
        add_subdirectory(${SDK_DIR}/hw/sysctrl)
        set(link_libs ${link_libs} sysctrl)
    endif()
    ### USE_PMON0 
    cmake_dependent_option(
        USE_PMON0 "Use PMON0" ON
        "NOT $CACHE{BOARD} STREQUAL QEMU;${ENABLE_USE_PMON0}" OFF
        )
    if(USE_PMON0)
        add_subdirectory(${SDK_DIR}/hw/pmon0)
        set(link_libs ${link_libs} pmon0)
    endif()

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
        ${compile_options_align}
        $<$<COMPILE_LANGUAGE:CXX>:${compile_options_cxx}>
)

### link options

target_link_options($CACHE{PROJECT_NAME}.elf
    PRIVATE
        ${compile_options_arch}
        ${link_options}
)

### link libs

target_link_libraries($CACHE{PROJECT_NAME}.elf
    PRIVATE 
        linkerscript
        -Wl,--start-group ${link_libs} -Wl,--end-group
        -Wl,--start-group ${app_libs} -Wl,--end-group
)

# Cumtom targets

## dump

add_custom_target($CACHE{PROJECT_NAME}.dump ALL
    COMMAND ${CMAKE_OBJDUMP} -Sx $CACHE{PROJECT_NAME}.elf > $CACHE{PROJECT_NAME}.dump
    DEPENDS $CACHE{PROJECT_NAME}.elf
    WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)

## bin

add_custom_target( $CACHE{PROJECT_NAME}.bin ALL
    COMMAND ${CMAKE_OBJCOPY} -O binary $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O verilog  $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.verilog.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND hexdump -e '"%08x\\n"' -v ${PROJECT_NAME}.bin > ${PROJECT_NAME}.hex
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
