CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=hello -DPROJECT_PATH=. -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM_AND_RAM
cmake --build build -j100