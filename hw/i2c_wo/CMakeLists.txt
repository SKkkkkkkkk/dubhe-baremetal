set(LIB_OBJS_NAME i2c_wo)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME}
	PRIVATE
	src/i2c_wo.c
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
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



# 依赖模块
target_link_libraries(${LIB_OBJS_NAME}
	PRIVATE
		systimer
)
