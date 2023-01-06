CROSS_COMPILE=arm-none-eabi- cmake -S ../../../ -B build -DPROJECT_NAME=rom2xxx -DPROJECT_PATH=. -DCORE=M3 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_RAM
cmake --build build -j100