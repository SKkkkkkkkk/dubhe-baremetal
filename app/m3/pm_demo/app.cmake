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
set(ENABLE_USE_PM ON)
set(ENABLE_USE_PMIC ON)
set(ENABLE_USE_I2C ON)
set(ENABLE_USE_DW_APB_TIMERS ON)
set(ENABLE_USE_PMU ON)
set(ENABLE_USE_I2C_WO ON)
