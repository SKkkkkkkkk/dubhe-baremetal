# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/openamp_demo.c
	${CMAKE_CURRENT_LIST_DIR}/src/rsc_table.c
)

# 添加app include dirs
set(app_inc
	/Volumes/Workspace/openamp/libmetal/build_m3_generic/install/usr/local/include/
	/Volumes/Workspace/openamp/open-amp/build_m3_generic/install/usr/local/include/
)


set(app_libs
	/Volumes/Workspace/openamp/libmetal/build_m3_generic/install/usr/local/lib/libmetal.a
	/Volumes/Workspace/openamp/open-amp/build_m3_generic/install/usr/local/lib/libopen_amp.a
)