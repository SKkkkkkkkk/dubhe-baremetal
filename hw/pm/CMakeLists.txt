set(LIB_NAME dw_apb_timers)

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
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
)
