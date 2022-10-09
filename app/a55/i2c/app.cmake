# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

set(ENABLE_USE_I2C ON)
set(ENABLE_USE_DW_APB_TIMERS ON)