# 添加app sources
set(app_src
	# ${CMAKE_CURRENT_LIST_DIR}/src/default_pon_m3.c
	# ${CMAKE_CURRENT_LIST_DIR}/src/pmu_on_off_rst_m3.c
	# ${CMAKE_CURRENT_LIST_DIR}/src/pmu_dyn_pwr_m3.c
	${CMAKE_CURRENT_LIST_DIR}/src/pmu_sleep_wakeup_m3.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
)

# 添加模块
set(ENABLE_USE_PMU ON)

