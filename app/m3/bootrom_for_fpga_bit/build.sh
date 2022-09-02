cmake -S ../../../ -B build -DPROJECT_NAME=bootrom_for_fpga_bit -DPROJECT_PATH=. -DCORE=M3 -DBOARD=RTL -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM_AND_RAM
cmake --build build -j100