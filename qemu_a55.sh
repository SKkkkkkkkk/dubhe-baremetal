qemu-system-aarch64 \
-M virt,secure=on,virtualization=on,gic-version=2 \
-nographic \
-cpu cortex-a53 \
-m 3G \
-smp 1 \
-d unimp \
-semihosting-config enable=on,target=native \
-bios build/a55_qemu/hello.bin \
-S -s