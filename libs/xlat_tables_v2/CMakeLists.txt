set(LIB_OBJS_NAME xlat_tables_v2)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		aarch64/enable_mmu.S
		aarch64/xlat_tables_arch.c
		xlat_tables_context.c
		xlat_tables_core.c
		xlat_tables_utils.c
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		include
		${SDK_DIR}/hw/cores/arm/armv8/a/cortex-a55/aarch64/inc
		${SDK_DIR}/common/inc
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_common}
		${compile_options_opt}
		${compile_options_align}
		-DENABLE_ASSERTIONS
)

