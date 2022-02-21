cmake -S . -B build_ubuntu -DCMAKE_TOOLCHAIN_FILE=../../../../tools/cmake_toolchains/aarch64gcc.cmake
cmake --build build_ubuntu -t -j100