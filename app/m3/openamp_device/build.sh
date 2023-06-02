CROSS_COMPILE=arm-none-eabi- cmake -S ../../../ -B build -DPROJECT_NAME=openamp_device -DPROJECT_PATH=. -DCORE=M3 -DBOARD=QEMU -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM
cmake --build build -j100