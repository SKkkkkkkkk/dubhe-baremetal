# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/fip_update/fip_update.c
	${CMAKE_CURRENT_LIST_DIR}/src/fip_update/uart_update.c
	${CMAKE_CURRENT_LIST_DIR}/src/tinyCLI_CMD.c
	${CMAKE_CURRENT_LIST_DIR}/src/setpc.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
	${CMAKE_CURRENT_LIST_DIR}/inc/fip_update
)

# 添加模块
set(ENABLE_USE_tinyCLI ON)
set(ENABLE_USE_CRC ON)
set(ENABLE_USE_xyzModem ON)
set(ENABLE_USE_DW_APB_SSI ON)
set(ENABLE_USE_NOR_FLASH ON)