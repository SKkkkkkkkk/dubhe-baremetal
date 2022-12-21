CROSS_COMPILE=arm-none-eabi- cmake -S ../../../ -B build -DPROJECT_NAME=m3_bl1 -DPROJECT_PATH=. -DCORE=M3 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM_AND_RAM
cmake --build build -j100