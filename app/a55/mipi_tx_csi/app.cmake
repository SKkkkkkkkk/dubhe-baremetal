# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/mipi_tx_csi.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
	${CMAKE_CURRENT_LIST_DIR}/src
)
