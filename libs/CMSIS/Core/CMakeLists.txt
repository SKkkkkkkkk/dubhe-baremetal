set(LIB_NAME cmsis_core)
add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME}
	INTERFACE
		Include
)