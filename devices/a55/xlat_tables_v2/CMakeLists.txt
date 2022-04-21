set(LIB_OBJS_NAME xlat_tables_v2)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		aarch64/enable_mmu.S
		aarch64/xlat_tables_arch.c
		xlat_tables_context.c
		xlat_tables_core.c
		xlat_tables_utils.c
)

# set(SDK_DIR ../../..)
target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		include
		${SDK_DIR}/devices/a55/arch/aarch64/inc
		${SDK_DIR}/include
)

target_compile_options(${LIB_OBJS_NAME}
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
		-DENABLE_ASSERTIONS
)