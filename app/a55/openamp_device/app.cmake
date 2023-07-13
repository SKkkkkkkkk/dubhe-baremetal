# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/openamp_demo.c
	${CMAKE_CURRENT_LIST_DIR}/src/rsc_table.c
)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
	# /Volumes/Workspace/openamp/libmetal/build_a55_generic/install/usr/local/include/
	# /Volumes/Workspace/openamp/open-amp/build_a55_generic/install/usr/local/include/
	/root/seehi/Dubhe/open-amp/libmetal/build_a55_generic/install/usr/local/include/
	/root/seehi/Dubhe/open-amp/build_a55_generic/install/usr/local/include/
)


set(app_libs
	# /Volumes/Workspace/openamp/libmetal/build_a55_generic/install/usr/local/lib/libmetal.a
	# /Volumes/Workspace/openamp/open-amp/build_a55_generic/install/usr/local/lib/libopen_amp.a
	/root/seehi/Dubhe/open-amp/libmetal/build_a55_generic/install/usr/local/lib/libmetal.a
	/root/seehi/Dubhe/open-amp/build_a55_generic/install/usr/local/lib/libopen_amp.a
)
