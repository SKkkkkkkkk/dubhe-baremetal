set(LIB_OBJS_NAME nor_flash)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		src/nor_flash.c
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
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

if($CACHE{CORE} STREQUAL "A55")
	target_link_libraries(${LIB_OBJS_NAME}
		PUBLIC
			aarch64_libc
	)
endif()

target_link_libraries(${LIB_OBJS_NAME}
	PRIVATE
		dw_apb_ssi
		systimer
)