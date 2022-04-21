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
		${SDK_DIR}/devices/common/CMSIS/Core/Include
	# PRIVATE

)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
)