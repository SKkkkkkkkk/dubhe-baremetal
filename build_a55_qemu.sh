cmake -S . -B build/a55_qemu -DDEVICE=A55 -DBOARD=QEMU -DPROJECT_NAME=hello -DPROJECT_PATH=app/hello_world
cmake --build build/a55_qemu -j100 