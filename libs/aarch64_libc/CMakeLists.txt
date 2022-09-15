set(LIB_OBJS_NAME aarch64_libc)

add_library( ${LIB_OBJS_NAME} STATIC )

target_sources( ${LIB_OBJS_NAME} 
	PRIVATE
		aarch64/setjmp.S
		abort.c
		# assert.c
		exit.c
		memchr.c
		memcmp.c
		memcpy.c
		memmove.c
		memrchr.c
		memset.c
		printf.c
		putchar.c
		puts.c
		snprintf.c
		strchr.c
		strcmp.c
		strlcat.c
		strlcpy.c
		strlen.c
		strncmp.c
		strnlen.c
		strrchr.c
		strtok.c
		strtol.c
		strtoll.c
		strtoul.c
		strtoull.c
)

target_include_directories( ${LIB_OBJS_NAME}
	PUBLIC
		inc
		inc/aarch64
		${SDK_DIR}/common/inc
)

target_compile_options( ${LIB_OBJS_NAME} 
	PRIVATE
	${compile_options_arch}
	${compile_options_board}
	${compile_options_common}
	${compile_options_opt}
	${compile_options_align}
	-mgeneral-regs-only
	-fno-PIE
)

target_link_libraries( ${LIB_OBJS_NAME}
	PUBLIC
		a55
)

if($CACHE{BOARD} STREQUAL "QEMU")
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			pl001
	)
else()
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			dw_apb_uart
	)
endif()