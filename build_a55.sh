cmake -S . -B build/a55 -DPROJECT_NAME=hello -DPROJECT_PATH=app/a55/hello -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug
bear cmake --build build/a55 --clean-first -j100