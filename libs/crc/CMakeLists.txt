set(LIB_OBJS_NAME crc)

add_library( ${LIB_OBJS_NAME} STATIC )

target_sources( ${LIB_OBJS_NAME} 
	PRIVATE
		src/crc16.c
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

if($CACHE{CORE} STREQUAL "A55")
	target_link_libraries(${LIB_OBJS_NAME}
		PUBLIC
			aarch64_libc
	)
elseif($CACHE{CORE} STREQUAL "M3")
	target_link_libraries(${LIB_OBJS_NAME}
		PUBLIC
	)
else()
	message(FATAL_ERROR "unsupport core!")
endif()