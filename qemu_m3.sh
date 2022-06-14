qemu-system-arm \
-cpu cortex-m3 \
-M lm3s6965evb \
-nographic \
-d unimp \
-semihosting-config enable=on,target=native \
-kernel build/m3/hello.elf \
-S -s
