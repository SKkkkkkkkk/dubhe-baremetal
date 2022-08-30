set(LIB_OBJS_NAME freertos_kernel)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		croutine.c
		event_groups.c
		list.c
		queue.c
		stream_buffer.c
		tasks.c
		timers.c
		portable/GCC/ARM_CM3/port.c
		portable/MemMang/heap_4.c

)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		include
		portable/GCC/ARM_CM3
	PRIVATE
		${app_inc}
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
		m3
)