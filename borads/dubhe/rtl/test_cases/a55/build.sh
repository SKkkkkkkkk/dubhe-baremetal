cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../../../../../tools/cmake_toolchains/aarch64gcc.cmake \
-DEXTRA_SRC=../../../../../borads/dubhe/rtl/hello_world/a55/src/main.c \
-DEXTRA_INC=../../../../../borads/dubhe/rtl/hello_world/a55/inc

cmake --build build -t -j100