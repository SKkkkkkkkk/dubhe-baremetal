set(LIB_NAME uart_16550)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} 
	PRIVATE
		src/aarch64/uart_16550.S
)

target_include_directories(${LIB_NAME}
	PUBLIC
		inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)

if($CACHE{CORE} STREQUAL "A55")
	target_link_libraries(${LIB_NAME}
		PUBLIC
			a55
	)
endif()