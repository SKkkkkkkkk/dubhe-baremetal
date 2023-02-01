# rm -rf build/*
# export PATH="$PATH:/home/wanghaodong/work/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-elf/bin"
CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=mipi_tx -DPROJECT_PATH=. -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_DDR
cmake --build build -j100
