set(LIB_NAME common)

add_library(${LIB_NAME} OBJECT)

file(GLOB_RECURSE C_SRCS src/*.c)
file(GLOB_RECURSE S_SRCS src/*.S)
target_sources(${LIB_NAME} 
	PRIVATE
		${S_SRCS}
		${C_SRCS}
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
		# -mstrict-align
		-DENABLE_ASSERTIONS
)