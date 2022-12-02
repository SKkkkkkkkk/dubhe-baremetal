set(LIB_NAME m3)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		src/startup.S
		src/system_m3.c
)

target_include_directories(${LIB_NAME}
	PUBLIC
		inc
		${SDK_DIR}/common/inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
)

add_subdirectory(${SDK_DIR}/libs/CMSIS/Core CMSIS/Core)
target_link_libraries(${LIB_NAME}
	PUBLIC
		cmsis_core
)


### link script
add_custom_command(
	OUTPUT 
		${CMAKE_CURRENT_BINARY_DIR}/m3.ld
	DEPENDS 
		${CMAKE_CURRENT_SOURCE_DIR}/src/m3.ld.S
		${CMAKE_CURRENT_SOURCE_DIR}/inc/mem_and_clock.h
	COMMAND 
		${CMAKE_CPP} -P -x assembler-with-cpp -D__LINKER__ -I${CMAKE_CURRENT_SOURCE_DIR}/inc/
		${compile_options_board}
		${CMAKE_CURRENT_SOURCE_DIR}/src/m3.ld.S
		-o ${CMAKE_CURRENT_BINARY_DIR}/m3.ld
	VERBATIM
)
add_custom_target(_linkerscript
	DEPENDS 
		${CMAKE_CURRENT_BINARY_DIR}/m3.ld
	VERBATIM
)

add_library(linkerscript INTERFACE)
add_dependencies(linkerscript _linkerscript)
target_link_options(linkerscript
	INTERFACE
		-T ${CMAKE_CURRENT_BINARY_DIR}/m3.ld
)
set_target_properties(linkerscript
	PROPERTIES INTERFACE_LINK_DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/m3.ld
)
