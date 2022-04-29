cmake -S . -B build/a55_fpga -DPROJECT_NAME=hello -DPROJECT_PATH=app/a55/hello -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug
bear cmake --build build/a55_fpga -j100