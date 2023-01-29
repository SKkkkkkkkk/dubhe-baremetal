set(LIB_OBJS_NAME freertos_kernel)

add_library(${LIB_OBJS_NAME} STATIC)


include (${CMAKE_CURRENT_LIST_DIR}/portable/portable.cmake)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		croutine.c
		event_groups.c
		list.c
		queue.c
		stream_buffer.c
		tasks.c
		timers.c
		${portable_src}
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		include
		${portable_inc}
	PRIVATE
		${app_inc}
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