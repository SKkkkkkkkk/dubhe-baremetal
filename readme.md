# Build

## Prerequisites

For building Dubhe bare-metal project you will need,at minimum:

- riscv gnu toolchain

- arm gnu toolchain

- cmake(3.15 or later)

- make

## Build A55

```shell
cmake -S . -B build/a55 -DDEVICE=A55 -DPROJECT_NAME=hello -DPROJECT_PATH=app/hello_world
cmake --build build/a55 -j100
```

## Build CV32E40P

```shell
cmake -S . -B build/cv32e40p -DDEVICE=CV32E40P -DPROJECT_NAME=hello -DPROJECT_PATH=app/hello_world
cmake --build build/cv32e40p -j100
```

## Build Options

- -S PATH - 选择cmake的顶层CMakelists.txt路径

- -B PATH - 选择生成的目标文件的地址

- -DDEVICE=XXX - 选择device，可选A55(默认)，CV32E40P，RI5CY

- -DBOARD=XXX - 选择board，可选RTL(默认)，FPGA，EVB

- -DPROJECT_NAME=XXX - 选择工程名(默认为工程名为a)

- -DPROJECT_PATH=XXX - 选择工程路径，路径下需包含app.cmake

- -DBIULD_TYPE=XXX - Debug, FastRel, MinSizeRel ...

- ...

 
