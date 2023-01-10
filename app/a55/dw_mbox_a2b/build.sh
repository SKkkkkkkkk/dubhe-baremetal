CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=dw_mailbox -DPROJECT_PATH=. -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_SYSRAM
cmake --build build -j100
