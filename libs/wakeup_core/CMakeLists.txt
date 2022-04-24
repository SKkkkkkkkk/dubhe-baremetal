set(LIB_NAME wakeup_core)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} 
	PRIVATE
		src/wakeup_core.c
)

target_include_directories(${LIB_NAME}
	INTERFACE
		inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		-mstrict-align
)