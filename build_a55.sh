cmake -S . -B build/a55 -DPROJECT_NAME=hello -DPROJECT_PATH=app/a55/hello -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug
cmake --build build/a55 -j100