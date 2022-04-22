set(LIB_OBJS_NAME newlib_port)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		newlib_port.c
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		# -mstrict-align
)

target_link_libraries(${LIB_OBJS_NAME}
	PRIVATE
		pl001
)