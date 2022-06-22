set(LIB_NAME cv32e40p)

add_library(${LIB_NAME} OBJECT) # must be OBJECT lib, because of syscalls.c.

target_sources(${LIB_NAME} 
	PRIVATE
		crt0.S
		handlers.S
		syscalls.c
		vectors.S
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)

### link script
add_custom_command(
	OUTPUT 
		${CMAKE_CURRENT_BINARY_DIR}/cv32e40p.ld
	DEPENDS 
		${CMAKE_CURRENT_SOURCE_DIR}/cv32e40p.ld.S
	COMMAND 
		arm-none-eabi-cpp -P -x assembler-with-cpp -D__LINKER__
		${compile_options_board}
		${CMAKE_CURRENT_SOURCE_DIR}/cv32e40p.ld.S
		-o ${CMAKE_CURRENT_BINARY_DIR}/cv32e40p.ld
	VERBATIM
)
add_custom_target(_linkerscript
	DEPENDS 
		${CMAKE_CURRENT_BINARY_DIR}/cv32e40p.ld
	VERBATIM
)

add_library(linkerscript INTERFACE)
add_dependencies(linkerscript _linkerscript)
target_link_options(linkerscript
	INTERFACE
		-T ${CMAKE_CURRENT_BINARY_DIR}/cv32e40p.ld
)
set_target_properties(linkerscript
	PROPERTIES INTERFACE_LINK_DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/cv32e40p.ld
)
