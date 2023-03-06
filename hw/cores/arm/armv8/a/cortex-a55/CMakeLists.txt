set(LIB_NAME a55)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		aarch64/src/entrypoint.S
		aarch64/src/exceptions.S
		aarch64/src/cache_helpers.S
		aarch64/src/config_mmu.c
)

target_include_directories(${LIB_NAME}
	PUBLIC
		aarch64/inc
		${SDK_DIR}/common/inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)


set(a55_sub_libs gic_v2 wakeup_core)
add_subdirectory(${SDK_DIR}/hw/gic/v2 gic_v2)
add_subdirectory(${SDK_DIR}/libs/wakeup_core wakeup_core)

if(NOT $CACHE{BOARD} STREQUAL QEMU)
	add_subdirectory(${SDK_DIR}/hw/system_counter system_counter)
	set(a55_sub_libs ${a55_sub_libs} system_counter)
endif()

target_link_libraries(${LIB_NAME}
	INTERFACE
		${a55_sub_libs}
)

### link script
add_custom_command(
	OUTPUT 
		${CMAKE_CURRENT_BINARY_DIR}/aarch64.ld
	DEPENDS 
		${CMAKE_CURRENT_SOURCE_DIR}/aarch64/src/aarch64.ld.S
	COMMAND 
		${CMAKE_CPP} -P -x assembler-with-cpp -D__LINKER__
		${compile_options_board}
		${CMAKE_CURRENT_SOURCE_DIR}/aarch64/src/aarch64.ld.S
		-o ${CMAKE_CURRENT_BINARY_DIR}/aarch64.ld
	VERBATIM
)
add_custom_target(_linkerscript
	DEPENDS 
		${CMAKE_CURRENT_BINARY_DIR}/aarch64.ld
	VERBATIM
)

add_library(linkerscript INTERFACE)
add_dependencies(linkerscript _linkerscript)
target_link_options(linkerscript
	INTERFACE
		-T ${CMAKE_CURRENT_BINARY_DIR}/aarch64.ld
)
set_target_properties(linkerscript
	PROPERTIES INTERFACE_LINK_DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/aarch64.ld
)
