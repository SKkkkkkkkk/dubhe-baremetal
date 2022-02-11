set(LIB_OBJS_NAME dubhe_a55)

add_library(${LIB_OBJS_NAME} OBJECT)

target_sources(${LIB_OBJS_NAME} 
	PRIVATE
		src/entrypoint.S
		src/exceptions.S
		# src/mmu_test.S
)

target_include_directories(${LIB_OBJS_NAME}
	PUBLIC
		inc
)

target_compile_options(${LIB_OBJS_NAME}
	PUBLIC
		-mlittle-endian -march=armv8-a
	PRIVATE
		-mstrict-align
		-Wall -Werror
		-ffunction-sections -fdata-sections
		$<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
		$<$<CONFIG:Release>:-Ofast -g0 -DNDEBUG>
)