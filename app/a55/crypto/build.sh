CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=crypto -DPROJECT_PATH=. -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM
cmake --build build -j100