set(LIB_OBJS_NAME systimer)

add_library( ${LIB_OBJS_NAME} STATIC )

target_sources( ${LIB_OBJS_NAME} 
	PRIVATE
		src/systimer_port.c
		src/systimer.c
)

target_include_directories( ${LIB_OBJS_NAME}
	PUBLIC
		inc
)

target_compile_options( ${LIB_OBJS_NAME} 
	PRIVATE
	${compile_options_arch}
	${compile_options_board}
	${compile_options_common}
	${compile_options_opt}
	${compile_options_align}
)

if($CACHE{BOARD} STREQUAL "QEMU")
	# target_link_libraries(${LIB_OBJS_NAME}
	# 	PRIVATE
	# 		pl001
	# )
else()
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			dw_apb_timers
			gic_v2
	)
endif()

if($CACHE{CORE} STREQUAL "A55")
	target_link_libraries(${LIB_OBJS_NAME}
		PUBLIC
			aarch64_libc
	)
endif()