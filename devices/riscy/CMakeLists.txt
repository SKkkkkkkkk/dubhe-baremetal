set(LIB_OBJS_NAME device_dubhe_riscv)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
	src/crt0.S
	src/spi_start.S
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
)

target_compile_options(${LIB_OBJS_NAME}
	PUBLIC
		-m32 -march=IMXpulpv2 -Wa,-march=IMXpulpv2
	PRIVATE
		-Wall -Werror 
		# -nostartfiles -nostdlib
		-ffunction-sections -fdata-sections
		$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
		$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
)