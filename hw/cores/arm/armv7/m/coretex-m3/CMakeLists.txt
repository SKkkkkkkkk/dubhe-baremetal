set(LIB_NAME m3)

add_library(${LIB_NAME} OBJECT)

target_sources(${LIB_NAME} 
	PRIVATE
		src/startup.S
		src/system_m3.c
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
)

add_subdirectory(${SDK_DIR}/libs/CMSIS/Core CMSIS/Core)
target_link_libraries(${LIB_NAME}
	PUBLIC
		cmsis_core
)
