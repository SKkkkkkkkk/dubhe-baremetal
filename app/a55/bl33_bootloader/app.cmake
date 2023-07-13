# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

# 添加模块
set(ENABLE_USE_DW_APB_SSI ON)
set(ENABLE_USE_NOR_FLASH ON)