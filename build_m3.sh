cmake -S . -B build/m3 -DPROJECT_NAME=hello -DPROJECT_PATH=app/m3/hello -DCORE=M3 -DBOARD=QEMU -DBUILD_TYPE=Debug
cmake --build build/m3 -j100