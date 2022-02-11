# qemu-system-aarch64.exe \
# -machine virt,secure=on,virtualization=on,gic-version=2 \
# -cpu cortex-a7 \
# -smp 2 \
# -bios ../APP/Debug/qemu_cortex-a7.elf \
# -device loader,addr=0x40000000,cpu-num=0 \
# -device loader,addr=0x40000000,cpu-num=1


# qemu-system-aarch64.exe \
# -machine virt,secure=off,virtualization=off,gic-version=2 \
# -cpu cortex-a7 \
# -m 3G \
# -smp 2 \
# -serial stdio \
# -kernel Release/qemu_cortex-a7.elf



qemu-system-aarch64 \
-serial stdio \
-machine virt,secure=on,virtualization=on,gic-version=2 \
-cpu cortex-a57 \
-m 3G \
-smp 4 \
-bios build/aarch64.bin

# -device loader,addr=0x42000000,file=Debug/qemu_cortex-a7.bin,force-raw=on \
# -kernel ../APP/Debug/qemu_cortex-a7.elf \