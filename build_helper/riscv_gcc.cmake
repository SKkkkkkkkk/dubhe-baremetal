set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

set( TOOLCHAIN $ENV{CROSS_COMPILE} )
set( CMAKE_C_COMPILER ${TOOLCHAIN}gcc )
set( CMAKE_CXX_COMPILER ${TOOLCHAIN}g++ )
set( CMAKE_ASM_COMPILER ${TOOLCHAIN}gcc )

set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )