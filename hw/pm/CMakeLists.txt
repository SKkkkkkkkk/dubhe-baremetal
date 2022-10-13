set(LIB_NAME pm)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} 
	PRIVATE
		src/cpu.s
		src/pm.c
		src/pm_rom.c
		src/port.c
		src/wakelock.c
)

target_include_directories(${LIB_NAME}
	PUBLIC
		inc
		${SDK_DIR}/common/inc
	PRIVATE
		${SDK_DIR}/libs/CMSIS/Core/Include
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)

target_link_libraries(${LIB_NAME}
	PUBLIC
		systimer
		m3
		freertos_kernel
)
