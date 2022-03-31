set(LIB_OBJS_NAME cv32e40p)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		crt0.S
		handlers.S
		syscalls.c
		vectors.S
)

# target_include_directories(${LIB_OBJS_NAME}
# 	PUBLIC
# )

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)