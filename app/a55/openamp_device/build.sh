CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=openamp_device -DPROJECT_PATH=. -DCORE=A55 -DPRIMARY_CORE=1 -DBOARD=QEMU -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_DDR
cmake --build build -j100