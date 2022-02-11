qemu-system-aarch64 \
-s -S \
-serial stdio \
-machine virt,secure=on,virtualization=on,gic-version=2 \
-cpu cortex-a57 \
-m 3G \
-smp 4 \
-bios build/aarch64.bin

# -bios debug/test.bin \

# qemu-system-aarch64.exe \
# -s -S \
# -machine virt,secure=on,virtualization=on,gic-version=2 \
# -cpu cortex-a7 \
# -m 3G \
# -smp 2 \
# -serial stdio \
# -kernel build/qemu_arm


# -bios all core处于running状态
# -kernel secondary core处于halted状态 