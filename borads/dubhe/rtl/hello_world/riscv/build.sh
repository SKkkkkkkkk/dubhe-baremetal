cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../../../../../tools/cmake_toolchains/riscv32_gcc.cmake
cmake --build build -t -j100