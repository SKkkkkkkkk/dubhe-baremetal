set(LIB_OBJS_NAME ri5cy)

add_library(${LIB_OBJS_NAME} STATIC)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		crt0.S
		spi_start.S
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
)

target_compile_options(${LIB_OBJS_NAME}
	# PUBLIC
	# 	-m32 -march=IMXpulpv2 -Wa,-march=IMXpulpv2
	# PRIVATE
	# 	-Wall -Werror 
	# 	# -nostartfiles -nostdlib
	# 	-ffunction-sections -fdata-sections
	# 	$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
	# 	$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
	PRIVATE
		${compile_options_arch}
		${compile_options_board}
		${compile_options_warning}
		${compile_options_opt}
		-mstrict-align
)