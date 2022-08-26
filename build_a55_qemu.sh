cmake -S . -B build/a55_qemu -DPROJECT_NAME=hello -DPROJECT_PATH=app/a55/hello -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug
cmake --build build/a55_qemu -j100