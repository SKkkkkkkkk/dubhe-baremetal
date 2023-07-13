# 添加app sources
set(app_src
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/image_header.c
	${CMAKE_SOURCE_DIR}/libs/micro-ecc/uECC.c
	${CMAKE_SOURCE_DIR}/libs/cifra/src/sha256.c
	${CMAKE_SOURCE_DIR}/libs/cifra/src/blockwise.c

)

# 添加app include dirs
set(app_inc
	${CMAKE_CURRENT_LIST_DIR}/inc
	${CMAKE_SOURCE_DIR}/libs/micro-ecc
	${CMAKE_SOURCE_DIR}/libs/cifra/src/ext
	${CMAKE_SOURCE_DIR}/libs/cifra/src
)