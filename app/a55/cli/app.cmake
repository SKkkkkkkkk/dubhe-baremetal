# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
	${CMAKE_CURRENT_LIST_DIR}/src/cli/
)

# 添加模块
set(ENABLE_USE_FREERTOS ON)
set(ENABLE_USE_tinyCLI ON)