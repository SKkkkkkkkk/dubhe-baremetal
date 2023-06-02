qemu-system-aarch64 \
-M virt,secure=on,virtualization=on,gic-version=2 \
-nographic \
-cpu cortex-a53 \
-m 3G \
-smp 2 \
-d unimp \
-semihosting-config enable=on,target=native \
-bios build/openamp_driver.bin \
-device loader,addr=0x50000000,cpu-num=1 \
-device loader,addr=0x50000000,file=../openamp_device/build/openamp_device.bin,force-raw=on \
-S -s

# -device loader,addr=0x00000000,cpu-num=0 \
# -device loader,addr=0x00000000,file=build/openamp_driver.bin,force-raw=on \



# -device loader,addr=0x00000000,file=build/openamp_driver.bin,force-raw=on \
# -bios build/openamp_driver.bin \
# -device loader,file=build/openamp_driver.elf \
# -device loader,addr=0x40000000,file=../openamp_device/build/openamp_device.bin,force-raw=on \