set(LIB_NAME a55)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} 
	PRIVATE
		aarch64/src/entrypoint.S
		aarch64/src/exceptions.S
		aarch64/src/cache_helpers.S
)


target_include_directories(${LIB_NAME}
	PUBLIC
		aarch64/inc
		${SDK_DIR}/common/inc
)

target_compile_options(${LIB_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		-mstrict-align
)

add_subdirectory(${SDK_DIR}/hw/gic/v2 gic_v2)
add_subdirectory(${SDK_DIR}/libs/wakeup_core wakeup_core)
target_link_libraries(${LIB_NAME}
	PUBLIC
		gic_v2
		wakeup_core
)