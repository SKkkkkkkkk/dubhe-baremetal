set(LIB_NAME a55)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		arch/aarch64/src/entrypoint.S
		arch/aarch64/src/exceptions.S
		arch/aarch64/inc/cache_helpers.S
		newlib/newlib_port.c
		# src/mmu_test.S
)

target_include_directories(${LIB_NAME}
	PUBLIC
		arch/aarch64/inc
	PRIVATE
		${SDK_DIR}/include
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)

add_subdirectory(xlat_tables_v2)
target_link_libraries(${LIB_NAME}
	PUBLIC
		xlat_tables_v2
)