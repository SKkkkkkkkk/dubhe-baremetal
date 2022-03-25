set(LIB_OBJS_NAME a55)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		src/entrypoint.S
		src/exceptions.S
		# src/mmu_test.S
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)