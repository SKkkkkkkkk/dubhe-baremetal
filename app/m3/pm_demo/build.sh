CROSS_COMPILE=arm-none-eabi- cmake -S ../../../ -B build -DPROJECT_NAME=pm_demo -DPROJECT_PATH=. -DCORE=M3 -DBOARD=RTL -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=MEMMAP_SCHEME_1
cmake --build build -j100