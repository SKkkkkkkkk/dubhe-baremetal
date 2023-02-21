# 添加app sources
set(app_src
	# ${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/i2c4_and_wo.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

# 添加模块
set(ENABLE_USE_I2C_WO ON)
set(ENABLE_USE_I2C ON)

