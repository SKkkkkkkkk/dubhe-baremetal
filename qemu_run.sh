qemu-system-aarch64 \
-M virt,secure=on,virtualization=on,gic-version=2 \
-cpu cortex-a57 \
-m 3G \
-serial stdio \
-d unimp \
-semihosting-config enable=on,target=native \
-bios build/a55/hello.elf \
-S -s