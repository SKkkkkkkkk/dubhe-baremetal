# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/openamp_demo.c
	${CMAKE_CURRENT_LIST_DIR}/src/rsc_table.c
)


set(ENABLE_USE_MAILBOX ON)
# 添加app include dirs
set(app_inc
	/root/seehi/Dubhe/open-amp/libmetal/build_m3_generic/install/usr/local/include/
	/root/seehi/Dubhe/open-amp/build_m3_generic/install/usr/local/include/
)


set(app_libs
	/root/seehi/Dubhe/open-amp/libmetal/build_m3_generic/install/usr/local/lib/libmetal.a
	/root/seehi/Dubhe/open-amp/build_m3_generic/install/usr/local/lib/libopen_amp.a
)
