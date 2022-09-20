cmake -S ../../../ -B build -DPROJECT_NAME=hello -DPROJECT_PATH=. -DCORE=M3 -DBOARD=RTL -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_DDR
cmake --build build