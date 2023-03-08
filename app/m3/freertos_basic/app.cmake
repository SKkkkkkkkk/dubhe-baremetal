# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.cc
	${CMAKE_CURRENT_LIST_DIR}/src/freertos_test.cc
	${CMAKE_CURRENT_LIST_DIR}/src/test.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/freertos_port.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

# 添加模块