set(LIB_OBJS_NAME a55)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		arch/aarch64/src/entrypoint.S
		arch/aarch64/src/exceptions.S
		newlib/newlib_port.c
		# src/mmu_test.S
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		arch/aarch64/inc
	PRIVATE
		${SDK_DIR}/include
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)