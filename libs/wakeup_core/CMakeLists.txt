set(LIB_OBJS_NAME wakeup_core)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		src/wakeup_core.c
)

target_include_directories(${LIB_OBJS_NAME}
	INTERFACE
		inc
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)

target_link_libraries(${LIB_OBJS_NAME}
	PUBLIC
		aarch64_libc
)