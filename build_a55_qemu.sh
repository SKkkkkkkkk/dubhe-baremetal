cmake -S . -B build/a55 -DDEVICE=A55 -DBOARD=QEMU -DPROJECT_NAME=hello -DPROJECT_PATH=app/hello_world
cmake --build build/a55 -j100 