set(LIB_NAME uart_16550)

add_library(${LIB_NAME} STATIC)

if($CACHE{CORE} STREQUAL "A55")
	set(sources src/aarch64/uart_16550.S)
else()
	set(sources src/aarch32/uart_16550.S)
endif()

target_sources(${LIB_NAME} 
	PRIVATE
		${sources}
)

target_include_directories(${LIB_NAME}
	PUBLIC
		inc
	PRIVATE
		${CMAKE_SOURCE_DIR}/common/inc
	
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