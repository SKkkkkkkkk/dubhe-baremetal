CROSS_COMPILE=arm-none-eabi- cmake -S ../../../ -B build -DPROJECT_NAME=freertos_basic -DPROJECT_PATH=. -DCORE=M3 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_DDR
cmake --build build