set(LIB_NAME cv32e40p)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		crt0.S
		handlers.S
		syscalls.c
		vectors.S
)

# target_include_directories(${LIB_NAME}
# 	PUBLIC
# )

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)