set(LIB_OBJS_NAME pmon0)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		src/pmon0.c
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
		${SDK_DIR}/common/inc
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)