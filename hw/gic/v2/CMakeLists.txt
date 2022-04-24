set(LIB_NAME gic_v2)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} 
	PRIVATE
		src/irq_ctrl_gic.c
)

target_include_directories(${LIB_NAME}
	PUBLIC
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