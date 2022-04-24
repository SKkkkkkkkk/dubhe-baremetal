# cmake -S . -B build/a55 -DPROJECT_NAME=hello -DPROJECT_PATH=app/a55/hello -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug
cmake -S . -B build/a55 -DPROJECT_NAME=hello -DEXTRA_SRC=app/a55/hello/src/main.c -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug
cmake --build build/a55 -j100