cmake -S ../../../ -B build -DPROJECT_NAME=dw_apb_ssi -DPROJECT_PATH=. -DCORE=A55 -DBOARD=FPGA -DBUILD_TYPE=Debug -DWHERE_TO_LOAD=ALL_IN_DDR
cmake --build build