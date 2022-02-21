cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../../../../tools/cmake_toolchains/aarch64gcc.cmake
cmake --build build -t -j100