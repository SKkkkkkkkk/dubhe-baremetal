set(LIB_NAME newlib_port)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		newlib_port.c
)

target_include_directories(${LIB_NAME}
	PRIVATE
		${SDK_DIR}/hw/uart/pl001/inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		# -mstrict-align
)

# target_link_libraries(${LIB_NAME}
# 	PRIVATE
# 		pl001
# )