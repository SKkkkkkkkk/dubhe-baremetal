set(LIB_OBJS_NAME seehi_printf)

add_library( ${LIB_OBJS_NAME} STATIC )

target_sources( ${LIB_OBJS_NAME} 
	PRIVATE
		seehi_print.c
		snprintf.c
)

target_include_directories( ${LIB_OBJS_NAME}
	PUBLIC
		.
	PRIVATE
		${SDK_DIR}/hal/uart/pl001
)


target_compile_options( ${LIB_OBJS_NAME} 
	PUBLIC
		-mlittle-endian -march=armv8-a
	PRIVATE
		-mstrict-align
		-Wall -Werror
		-ffunction-sections -fdata-sections
		$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
		$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
)