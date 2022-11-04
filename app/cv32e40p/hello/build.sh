CROSS_COMPILE=riscv64-unknown-elf- cmake -S ../../../ -B build -DPROJECT_NAME=hello -DPROJECT_PATH=. -DCORE=CV32E40P -DBOARD=RTL -DBUILD_TYPE=Debug
cmake --build build