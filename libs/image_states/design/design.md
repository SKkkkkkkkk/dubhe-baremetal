# 关于image_states

image_states用于指示当前系统处于哪个状态下。目前使用SYSCTRL寄存器中复位值为0的5个32bit寄存器用于指示当前A55 4个cores和M3处于的状态。

# Image_States寄存器

5个32bit系统寄存器字段定义相同，如下图:

![](Image_States_Register.png)

image[31:28]:定义当前处于的哪个image阶段(或者最近一次更新是在哪个阶段)。

states[27:0]:定义处于当前image阶段中的哪个state中。

# 预定义的image_states值

Image_States寄存器定义了一种比较广泛通用的状态表示方法。目前系统中我们预定义了一些状态，后续开发需要保持兼容性。

## A55预定义值

image[31:38]一共可以定义[0,15] 16个images，目前已经预定义使用了的是[0,3] 4个images。

```c
// 见libs/image_states/inc/image_states.h
typedef enum {
    BL1 = 0,
    BL2,
    BL31,
    NS_BL1U,
    A55_IMAGE_MAX
} a55_image_name;
```

一些预定义和已使用的值：

1. 0x00_000000 - 冷启动BL1阶段/(寄存器复位值)
2. 0x00_000001 - 冷启动secondary cores低功耗模式(等待CPU_ON唤醒)
3. 0x01_000000 - BL2阶段
4. 0x02_000000 - BL31阶段, Running态(4个低功耗状态(2,5,6,7)恢复正常之后都应该设置为此状态)
5. 0x02_000001 - BL31阶段, CPU_SUSPEND中的standby态
6. 0x02_000002 - BL31阶段, CPU_SUSPEND中的powerdown态(Warm Reset标志之一)
7. 0x02_000003 - BL31阶段, CPU_OFF态
8. 0x02_000004 - BL31阶段, 处于被CPU_ON状态(Warm Reset标志之一)

## M3预定义值

image[31:38]一共可以定义[0,15] 16个images，目前已经预定义使用了的是[0,3] 4个images。

```c
// 见libs/image_states/inc/image_states.h
typedef enum {
	M3_BL1 = 0,
	M3_BL2,
	M3_IMAGE_MAX
} m3_image_name;
```

一些预定义和已使用的值：

1. 0x00_000000 - 冷启动M3_BL1初始阶段/(寄存器复位值)

2. 0x00_000001 - M3_BL1等待运行M3_BL2状态(wfi低功耗状态)

3. 0x01_000000 - M3_BL2阶段
