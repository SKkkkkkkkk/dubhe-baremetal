qemu-system-riscv32 \
-M virt \
-nographic \
-m 3G \
-smp 1 \
-d unimp \
-bios build/cv32e40p/hello.bin \
-S -s