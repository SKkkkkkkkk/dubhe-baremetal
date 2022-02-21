# build

## pre-complie

默认使用linux host环境进行编译：

## toolchain

1. 下载解压aarch64 elf bare-metal target(aarch64-none-elf)工具链
2. 将gcc-arm-none-eabi-10.3-2021.10/bin/添加到环境变量PATH中
3. 最终效果为终端任意路径下运行 arch64-none-elf-gcc -v 成功

## build system

需要cmake和make

ubuntu/debian下：

```shell
sudo apt install cmake
sudo apt install make
```

### complie

1. 切换到对应的board的相关工程

2. 生成makefile. 

   ```shell
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../../../../../tools/cmake_toolchains/aarch64gcc.cmake
   ```

   1. -S . 顶层CMakeLists.txt目录
   2. -B build 将中间和目标文件放入build目录
   3. -DCMAKE_TOOLCHAIN_FILE=../../../../../tools/cmake_toolchains/aarch64gcc.cmake 选择跨平台编译器

3. 开始build目标文件. 

   ```shell
   cmake --build build
   // 上述命令等价于
   // 1. cd build
   // 2. make -j100
   ```

### build options

下面是一些可选的cmake命令行options：

1. CROSS_COMPILER_PATH
   1. 编译器路径
   2. 设置为空或者不设置，需要将编译器路径加入PATH
2. EXTRA_SRC
   1. 编译额外的.c文件
3. EXTRA_INC
   1. 额外的头文件路径



# 目录结构

## boards

存放板级设备，如：evb，fpga，rtl，qemu。rtl，qemu比较特殊，用于仿真

每个板级目录下面又可以存放各种应用级目录和文件

## devices

存放芯片级(soc级)文件的目录，如ape1210，dubhe等

## doc

可以存放一些文档

## hal

外设驱动文件目录

## tools

存放一些工具程序和文件
