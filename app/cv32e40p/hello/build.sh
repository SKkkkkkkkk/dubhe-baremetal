CROSS_COMPILE=riscv32-unknown-elf- cmake -S ../../../ -B build -DPROJECT_NAME=hello -DPROJECT_PATH=. -DCORE=CV32E40P -DBOARD=FPGA -DBUILD_TYPE=Debug
cmake --build build