CROSS_COMPILE=aarch64-none-elf- cmake -S ../../../ -B build -DPROJECT_NAME=ddr -DPROJECT_PATH=. -DCORE=A55 -DBOARD=EVB -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_SYSRAM
cmake --build build -j100


# CROSS_COMPILE=aarch64-none-elf- bear --  cmake -S ../../../ -B build -DPROJECT_NAME=ddr -DPROJECT_PATH=. -DCORE=A55 -DBOARD=EVB -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_SYSRAM -DCMAKE_C_COMPILER="/opt/homebrew/Cellar/bear/3.0.21/lib/bear/wrapper.d/aarch64-none-elf-gcc"
# bear -- cmake --build build -j100