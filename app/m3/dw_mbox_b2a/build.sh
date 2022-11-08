cmake -S ../../../ -B build -DPROJECT_NAME=dw_mailbox -DPROJECT_PATH=. -DCORE=M3 -DBOARD=RTL -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_RAM
cmake --build build
