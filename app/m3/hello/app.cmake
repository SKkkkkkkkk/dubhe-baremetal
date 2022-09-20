# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/freertos_test.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

# 添加模块
# set(ENABLE_USE_PL001 ON)
# set(ENABLE_USE_DW_APB_UART ON)
# set(ENABLE_USE_DW_APB_TIMERS OFF)
set(ENABLE_USE_FREERTOS ON)