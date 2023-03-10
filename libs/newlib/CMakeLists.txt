set(LIB_OBJS_NAME newlib_port)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		newlib_port.c
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)


if($CACHE{BOARD} STREQUAL "QEMU")
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			pl011
	)
else()
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			dw_apb_uart
	)
endif()

if($CACHE{OS} STREQUAL "FreeRTOS")
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			freertos_kernel
	)
endif()

if($CACHE{CORE} STREQUAL "A55")
	target_link_libraries(${LIB_OBJS_NAME}
		PRIVATE
			a55
	)
endif()