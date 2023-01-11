#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"
#include "pinmux.h"
#include "dw_apb_gpio.h"
#include "dw_apb_timers.h"
#include "arch_features.h"
#include "axp2101.h"
#include "i2c_wo.h"

int                      gic_cnt      = 0;
int                      err_cnt      = 0;
int                      count        = 0;
static volatile uint32_t timerx2_t1_i = 0U;
#define GPIO0            GPIO0_BASE
#define GPIO1            GPIO1_BASE
#define GIC_INTREFACE    0
#define TIMER_WAKEUP     0
#define GPIO_WAKEUP      1
#define I2C_4            0
#define I2C_WO           1
#define STOP             1

#define TIMER_FREQ       (20000000)

#define WAKEUP_PIN       16
#define WAKEUP_PIN_GROUP (WAKEUP_PIN / 32)
#define WAKEUP_PIN_NUM   (WAKEUP_PIN % 32)

#define WAKEUP_PIN1       50
#define WAKEUP_PIN_GROUP1 (WAKEUP_PIN / 32)
#define WAKEUP_PIN_NUM1   (WAKEUP_PIN % 32)

void set_power_off_a55(uint8_t pid)
{
    // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
    if (pid == AP) {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR,
                    (1 << PPU_PWPR_OP_DYN_EN_LSB |
                     OP0 << PPU_PWPR_OP_POLICY_LSB |
                     1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
    } else if (pid < AP && pid >= CORE0) {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR,
                    (1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
    } else {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR, OFF);
    }
}

void set_power_on_a55(uint8_t pid)
{
    // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
    if (pid == AP) {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR,
                    (1 << PPU_PWPR_OP_DYN_EN_LSB |
                     OP4 << PPU_PWPR_OP_POLICY_LSB |
                     1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    } else if (pid < AP && pid >= CORE0) {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR,
                    (1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    } else {
        set_pmu_reg(pid, PPU_PWPR_OP_DYN_EN_ADDR, ON);
    }
}

void POWER_OFF_SEQ(void)
{
    __asm("mrs x0, s3_0_c15_c2_7\n"
          "orr x0, x0, #0x1\n"
          "msr s3_0_c15_c2_7, x0\n"
          "isb\n"
          "wfi\n");
}

#define SYS_RESET_0      (SYS_BASE + 0xff0)
#define SYS_RESET_1      (SYS_BASE + 0xff4)
#define SYS_RESET_2      (SYS_BASE + 0xff8)
#define SYS_RESET_3      (SYS_BASE + 0xffc)

#define SYS_RESET_MAGIC0 (0x5a5a5a00)
#define SYS_RESET_MAGIC1 (0x5a5a5a01)
#define SYS_RESET_MAGIC2 (0x5a5a5a02)
#define SYS_RESET_MAGIC3 (0x5a5a5a03)

void core1_c_entry(void)
{
    printf("hello_world core1\n");
    printf("core1 0x2e000ff4 = 0x%x\n", REG32(SYS_RESET_1));
    if (REG32(SYS_RESET_1) == 0) {
        REG32(SYS_RESET_1) = SYS_RESET_MAGIC1;
#if GIC_INTREFACE
        GIC_DisableInterface();
#endif
        set_power_off_a55(CORE1);
        uint32_t pwsr = 1;
        while (1) {
            pwsr = get_pmu_reg(CORE1, PPU_PWPR_OP_DYN_EN_ADDR);
            if (((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
                printf("start enter wfi core1\n");
                break;
            }
        }
        POWER_OFF_SEQ();
    } else {
#if GIC_INTREFACE
        GIC_EnableInterface();
#endif
        printf("core1 is wakeup\n");
    }
    while (1)
        ;
}

void core2_c_entry(void)
{
    printf("hello_world core2\n");
    printf("core2 0x2e000ff8 = 0x%x\n", REG32(SYS_RESET_2));
    if (REG32(SYS_RESET_2) == 0) {
        REG32(SYS_RESET_2) = SYS_RESET_MAGIC2;
#if GIC_INTREFACE
        GIC_DisableInterface();
#endif
        set_power_off_a55(CORE2);
        uint32_t pwsr = 1;
        while (1) {
            pwsr = get_pmu_reg(CORE2, PPU_PWPR_OP_DYN_EN_ADDR);
            if (((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
                printf("start enter wfi core2\n");
                break;
            }
        }
        POWER_OFF_SEQ();
    } else {
#if GIC_INTREFACE
        GIC_EnableInterface();
#endif
        printf("core2 is wakeup\n");
    }
    while (1)
        ;
}

void core3_c_entry(void)
{
    printf("hello_world core3\n");
    printf("core3 0x2e000ffc = 0x%x\n", REG32(SYS_RESET_3));
    if (REG32(SYS_RESET_3) == 0) {
        REG32(SYS_RESET_3) = SYS_RESET_MAGIC3;
#if GIC_INTREFACE
        GIC_DisableInterface();
#endif
        set_power_off_a55(CORE3);
        uint32_t pwsr = 1;
        while (1) {
            pwsr = get_pmu_reg(CORE3, PPU_PWPR_OP_DYN_EN_ADDR);
            if (((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
                printf("start enter wfi core3\n");
                break;
            }
        }
        POWER_OFF_SEQ();
    } else {
#if GIC_INTREFACE
        GIC_EnableInterface();
#endif
        printf("core3 is wakeup\n");
    }
    while (1)
        ;
}

void irq_handler_gpio0(void)
{
    uint32_t intstatus = *((uint32_t *) (GPIO0 + 0x40));

    count++;

    if (intstatus & (1 << WAKEUP_PIN_NUM)) {
        gpio_clear_interrput(WAKEUP_PIN_GROUP, WAKEUP_PIN_NUM);
        printf("////////////////gpio in irq 0. %d/////////////////\n", count);
    } else {
        printf("irq is not 0\n");
    }

    return;
}

void set_gpio_wakeup(void)
{
    pinmux(WAKEUP_PIN, 7); // gpio0_0

    void irq_handler_gpio0(void);
    GIC_SetTarget(GPIO0_IRQn, 1 << 0);
    IRQ_SetHandler(GPIO0_IRQn, irq_handler_gpio0);
    IRQ_SetPriority(GPIO0_IRQn, 0 << 3);
    IRQ_Enable(GPIO0_IRQn);

    gpio_init_config_t gpio_init_config = {
        .group             = WAKEUP_PIN_GROUP,
        .pin               = WAKEUP_PIN_NUM,
        .gpio_control_mode = Software_Mode,
        .gpio_mode         = GPIO_Low_Int_Mode,
        // .gpio_mode = GPIO_Input_Mode,
    };
    gpio_init(&gpio_init_config);
}

void irq_handler_gpio1(void)
{
    uint32_t intstatus = *((uint32_t *) (GPIO1 + 0x40));

    count++;

    if (intstatus & (1 << WAKEUP_PIN_NUM1)) {
        gpio_clear_interrput(WAKEUP_PIN_GROUP1, WAKEUP_PIN_NUM1);
        printf("////////////////gpio in irq 1. %d/////////////////\n", count);
    } else {
        printf("irq is not 0\n");
    }

    return;
}

void set_gpio1_wakeup(void)
{
    pinmux(50, 7); // gpio0_0

    void irq_handler_gpio1(void);
    GIC_SetTarget(GPIO1_IRQn, 1 << 0);
    IRQ_SetHandler(GPIO1_IRQn, irq_handler_gpio1);
    IRQ_SetPriority(GPIO1_IRQn, 0 << 3);
    IRQ_Enable(GPIO1_IRQn);

    gpio_init_config_t gpio_init_config = {
        .group             = 1,
        .pin               = 18,
        .gpio_control_mode = Software_Mode,
        .gpio_mode         = GPIO_Low_Int_Mode,
        // .gpio_mode = GPIO_Input_Mode,
    };
    gpio_init(&gpio_init_config);
}

void timerx2_t1_irqhandler(void)
{
    (void) TIMERX2->Timer1EOI;
    ++timerx2_t1_i;
    printf("///////////////timerx2_t1_irqhandler in irq %d./////////////\n",
           timerx2_t1_i);
}

void set_timerx2_wakeup(void)
{
    void timerx2_t1_irqhandler(void);
    GIC_SetTarget(Timerx2_T1_IRQn, 1 << 0); // core0
    IRQ_SetHandler(Timerx2_T1_IRQn, timerx2_t1_irqhandler);
    IRQ_SetPriority(Timerx2_T1_IRQn, 0 << 3);
    IRQ_Enable(Timerx2_T1_IRQn);

    timer_init_config_t timer_init_config = {.int_mask   = 0,
                                             .loadcount  = (5) * TIMER_FREQ,
                                             .timer_id   = Timerx2_T1,
                                             .timer_mode = Mode_User_Defined};
    timer_init(&timer_init_config);
}

void set_power_off_seq(void)
{
    set_pmu_reg(PMU, PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,
                ~(1 << PMU_IMR_PMU_WAKEUP_1_MASK_LSB |
                  1 << PMU_IMR_PPU_DDR0_IRQ_MASK_LSB));
    set_pmu_reg(PMU, PMU_PDSEQ_0_LOGICID_ADDR, (NPU1 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_1_LOGICID_ADDR, (NPU0 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_2_LOGICID_ADDR, (DEWARP << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_3_LOGICID_ADDR, (ISP << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_4_LOGICID_ADDR, (IMG << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_5_LOGICID_ADDR, (VPU << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_6_LOGICID_ADDR, (GPU << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_7_LOGICID_ADDR, (PERI1 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_8_LOGICID_ADDR, (LP << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_9_LOGICID_ADDR, (DDR1 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_10_LOGICID_ADDR, (DDR0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_11_LOGICID_ADDR,(PERI0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_12_LOGICID_ADDR,(CORE3 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_13_LOGICID_ADDR,(CORE2 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_14_LOGICID_ADDR,(CORE1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_15_LOGICID_ADDR,(CORE0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_16_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU, PMU_PD_CR_NUM_PD_ADDR, (11 << 4 | 1));
}

void set_default_power_on_seq(void)
{
    set_pmu_reg(PMU, PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,
                ~(1 << PMU_IMR_PPU_DDR1_IRQ_MASK_LSB));
    set_pmu_reg(PMU, PMU_PUSEQ_0_LOGICID_ADDR, (LP << 8 | ON));
    set_pmu_reg(PMU, PMU_PUSEQ_1_LOGICID_ADDR, (PERI1 << 8 | ON));
    set_pmu_reg(PMU, PMU_PUSEQ_2_LOGICID_ADDR, (DDR0 << 8 | ON));
    set_pmu_reg(PMU, PMU_PUSEQ_3_LOGICID_ADDR, (DDR1 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(PERI0 << 8 | ON));
    set_pmu_reg(PMU, PMU_PU_CR_NUM_PD_ADDR, (4 << 4 | 1));
}

void pmu_irqhandler(void)
{
    uint32_t exp;
    printf("NOTE PMU interrupt occurred! gic_cnt:%d\n", gic_cnt);
    // IRQ_Disable(INT_PMU_INTR);
    uint32_t tmp = get_pmu_reg(PMU, PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x\n", tmp);
    if (gic_cnt == 0) {
        // default power on interrupt
        exp = get_pmu_isr(PMU);
        if (((tmp >> PMU_IMR_PMU_WAKEUP_1_MASK_LSB) & 0x1) == 1) {
            gic_cnt = 2; // jump gic_cnt=2 power off operation
            clear_all_ppu_isr();
        } else if (tmp == exp) {
            printf("NOTE all default power on PD power up\n");
            // check each pd ISR and clear PPU_ISR
            for (int i = 0; i < PDNUM; i++) {
                if (((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR,
                                             get_pmu_default_pwsr(i));
                }
            }
        } else {
            err_cnt++;
        }
        set_pmu_reg(PMU, PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, 0xffffffff);
    } else if (gic_cnt == 1) {
        exp = 0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB | 0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB | 0x1 << PMU_ISR_PPU_VPU_IRQ_LSB;
        if ((tmp & exp) == exp) {
            printf("NOTE all off PD power up\n");
            for (int i = 0; i < PDNUM; i++) {
                if (((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR,
                                             get_pmu_pwsr(i, ON + OP4));
                    err_cnt += check_pmu_reg(
                        PMU, PMU_PD_CORE0_CR_PPU_PSTATE_ADDR + i * 4,
                        get_pmu_cr(i, ON + OP4));
                }
            }
        } else {
            err_cnt++;
        }
    } else if (gic_cnt == 3) {
        // exp = 0x19600;//get_pmu_isr(PMU);
        exp = // 0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              //  0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
            0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB | 0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
            // 0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
            0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
            // 0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
            // 0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
            // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
            0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if (tmp == exp) {
            printf("NOTE all default power on PD power up again\n");
            for (int i = 0; i < PDNUM; i++) {
                if (i == AP || i == CORE0 || i == CORE1 || i == CORE2 ||
                    i == CORE3) {
                    // err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,0);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR,
                                             get_pmu_pwsr(i, ON + OP4));
                    err_cnt += check_pmu_reg(
                        PMU, PMU_PD_CORE0_CR_PPU_PSTATE_ADDR + i * 4,
                        get_pmu_cr(i, ON + OP4));
                } else if (((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR,
                                             get_pmu_pwsr(i, ON + OP4));
                    err_cnt += check_pmu_reg(
                        PMU, PMU_PD_CORE0_CR_PPU_PSTATE_ADDR + i * 4,
                        get_pmu_cr(i, ON + OP4));
                } else if (i == PERI0) {
                    continue;
                } else {
                    err_cnt += check_pmu_irq(i, 0x0, 0xffffffff, 0);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR,
                                             get_pmu_pwsr(i, OFF));
                    err_cnt += check_pmu_reg(
                        PMU, PMU_PD_CORE0_CR_PPU_PSTATE_ADDR + i * 4,
                        get_pmu_cr(i, OFF));
                }
            }
        } else {
            err_cnt++;
        }
    }
    gic_cnt++;
    set_pmu_reg(PMU, PMU_ISR_PMU_WAKEUP_5_ADDR, 0xffffffff);
}

int main(void)
{
    printf("test %s ...\n", __FILE__);
    systimer_init();

    uint32_t tmp;

#if I2C_4
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;

    int err = 0;
    // unsigned int val;
    struct pmic_cfg cfg;

    strcpy(cfg.name, "axp2101");
    cfg.reg_addr   = 0x34;
    cfg.i2c_bus    = 4;
    cfg.check_addr = 0x00;
    cfg.check_len  = 1;
    err            = axp2101_i2c_init(&cfg);

    if (!err) {
        axp20x_i2c_write(AXP2101_INTEN1, 0);
        axp20x_i2c_write(AXP2101_INTEN2, 0);
        axp20x_i2c_write(AXP2101_INTEN3, 0);
        // axp2101_pmic_ops.pmic_to_sleep  = axp2101_powerkey_suspend;
        // axp2101_pmic_ops.pmic_to_resume = axp2101_powerkey_resume;
    }
    // __asm__ volatile("b .");

    systimer_delay(1, IN_S);
    printf("axp2101_powerkey_suspend !!! \n");
    // axp2101_powerkey_suspend();
#endif

#if I2C_WO
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(2000); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x09 | STOP << 8);
    i2c_wo_fifo(0x80);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x90);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x91);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x19 | STOP << 8);
    i2c_wo_fifo(0x41);
    i2c_wo_fifo(0x0c | STOP << 8);
    i2c_wo_start();

	GIC_DistInit();
	GIC_CPUInterfaceInit(); //per CPU
	set_gpio_wakeup();
	set_gpio1_wakeup();
	set_timerx2_wakeup();
	timer_enable(Timerx2_T1);

    while (1) {
        printf("i2c_wo_status 0x%x\n", i2c_wo_status());
        printf("i2c_wo_status_fifo_level 0x%x\n", i2c_wo_status_fifo_level());
		printf("0x25100848 = 0x%x\n", REG32(0x25100848));
		printf("0x25030000 = 0x%x\n", REG32(0x25030000));
		printf("0x25030004 = 0x%x\n", REG32(0x25030004));
		printf("0x25030008 = 0x%x\n", REG32(0x25030008));
		printf("0x25030030 = 0x%x\n", REG32(0x25030030));
		printf("0x25030050 = 0x%x\n", REG32(0x25030050));
        // axp20x_i2c_read(AXP2101_SLEEP_CFG, &val);
        // printf("AXP2101_SLEEP_CFG = 0x%x\n", val);
        systimer_delay(1, IN_S);
    }
#endif

#if 0
	uint32_t exp;

	set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));

	printf("core0 0x2e000ff0 = 0x%x\n", REG32(SYS_RESET_0));
	if( REG32(SYS_RESET_0) == 0){
		printf("core0 is cold boot\n");

		REG32(SYS_RESET_0) = SYS_RESET_MAGIC0;
		//open each PPU ISR
		exp = get_pmu_isr(PMU);
		tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
		while(tmp != exp){
			tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
		}

		for(int i=0; i<PDNUM; i++) {
			if(((exp >> i) & 1) == 1) {
				err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
				err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_default_pwsr(i));
			}
		}
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);

		GIC_DistInit();
		GIC_CPUInterfaceInit(); //per CPU

		// void pmu_irqhandler(void);
		// GIC_SetTarget(PMU_IRQn, 1 << 0); //core0
		// IRQ_SetHandler(PMU_IRQn, pmu_irqhandler);
		// IRQ_SetPriority(PMU_IRQn, 0 << 3);
		// IRQ_Enable(PMU_IRQn);
		
		printf("axp2101_powerkey_suspend !!! \n");
		axp2101_powerkey_suspend();

		gic_cnt = 2;
	}else{
		printf("core0 is wakeup 0x%x\n", get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR));
#if GIC_INTREFACE
		GIC_EnableInterface();
#endif

		printf("axp2101_powerkey_resume !!! \n");
		axp2101_powerkey_resume();
		// clear_all_ppu_isr();

		GIC_DistInit();
		GIC_CPUInterfaceInit(); //per CPU

		set_power_on_a55(3);
		wakeup_core(3, core3_c_entry); //core3 enter to WFI
		systimer_delay(1, IN_S);
		set_power_on_a55(2);
		wakeup_core(2, core2_c_entry); //core2 enter to WFI
		systimer_delay(1, IN_S);
		set_power_on_a55(1);
		wakeup_core(1, core1_c_entry); //core1 enter to WFI
		systimer_delay(1, IN_S);

		for(int i=0; i<PDNUM; i++) {
			if(i == AP || i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3) {
				err_cnt += check_pmu_irq(i,0x87,0xffffffff,0);
				err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
				err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
			}
		}

		void pmu_irqhandler(void);
		// GIC_SetTarget(PMU_IRQn, 1 << 0); //core0
		// IRQ_SetHandler(PMU_IRQn, pmu_irqhandler);
		// IRQ_SetPriority(PMU_IRQn, 0 << 3);
		// IRQ_Enable(PMU_IRQn);
#if TIMER_WAKEUP
		// printf("wakeup TimersRawIntStatus 0x%x\n", TIMERX2->TimersRawIntStatus);
		// printf("wakeup TimersIntStatus 0x%x\n", TIMERX2->TimersIntStatus);
		GIC_SetTarget(Timerx2_T1_IRQn, 1 << 0); //core0
		IRQ_SetHandler(Timerx2_T1_IRQn, timerx2_t1_irqhandler);
		IRQ_SetPriority(Timerx2_T1_IRQn, 0 << 3);
		IRQ_Enable(Timerx2_T1_IRQn);
#endif
#if GPIO_WAKEUP
		GIC_SetTarget(GPIO0_IRQn, 1 << 0);
		IRQ_SetHandler(GPIO0_IRQn, irq_handler_gpio0);
		IRQ_SetPriority(GPIO0_IRQn, 0 << 3);
		IRQ_Enable(GPIO0_IRQn);
#endif
		gic_cnt = 3;
	}

    printf("NOTE gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);

	if(gic_cnt == 2){
		set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, 0); //clear PU_EN
#if GPIO_WAKEUP
		set_gpio_wakeup();
#endif
#if TIMER_WAKEUP
		set_timerx2_wakeup();
#endif
		set_pmu_wakeup(0, 0x44); //set wakeup timer target:a55 core0
		set_pmu_wakeup(1, 0x44); //set wakeup gpio0_16 target:a55 core0
#if TIMER_WAKEUP
		timer_enable(Timerx2_T1);
		systimer_delay(6, IN_S);
#endif
        wakeup_core(3, core3_c_entry); //core3 enter to WFI
		systimer_delay(100, IN_MS);
        wakeup_core(2, core2_c_entry); //core2 enter to WFI
		systimer_delay(100, IN_MS);
        wakeup_core(1, core1_c_entry); //core1 enter to WFI
		systimer_delay(100, IN_MS);
		set_power_off_a55(DDR0);
		// set_power_off_seq();
        // seehi_cmd(0xff000000);
#if GIC_INTREFACE
		GIC_DisableInterface();
#endif
		set_power_off_a55(AP);
		set_power_off_a55(CORE0);
		printf("core0 enter wfi !!\n");
		POWER_OFF_SEQ(); //core0 enter WFI
      } else if(gic_cnt == 3) {
#if GPIO_WAKEUP
		IRQ_Disable(GPIO0_IRQn);
#endif
#if TIMER_WAKEUP
		IRQ_Disable(Timerx2_T1_IRQn);
#endif
		set_power_on_a55(DDR0);
        set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, 0); //clear PD_EN
		printf("set_default_power_on_seq !!\n");
        // set_default_power_on_seq();
      }

    if(err_cnt == 0) {
        TEST_PASS;
    } else {
        printf("ERROR err_cnt is %d\n", err_cnt);
        TEST_FAIL;
    }
    return 0;
#endif
}

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
