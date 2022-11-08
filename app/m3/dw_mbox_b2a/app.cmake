# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/../../../hw/mbox/src/mailbox.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/../../../hw/mbox/inc
)

# 添加模块
set(ENABLE_USE_SYSTIMER ON)
