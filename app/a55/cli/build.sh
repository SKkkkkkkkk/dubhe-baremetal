cmake -S ../../../ -B build -DPROJECT_NAME=cli -DPROJECT_PATH=. -DCORE=A55 -DBOARD=QEMU -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ROM_AND_RAM
cmake --build build