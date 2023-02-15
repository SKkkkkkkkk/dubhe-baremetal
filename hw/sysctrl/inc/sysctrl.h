#ifndef __SYSCTRL_H__
#define __SYSCTRL_H__
/*level:
 * 0:set EMMC1 CLK/CMD/D0~7/PWR/RSTN pinmux
 * 1:set EMMC1 CLK/CMD/D0~3/PWR/RSTN pinmux
 * 2:set EMMC1 CLK/CMD/D0~3 pinmux
 */
int set_emmc1_pinmux(int level);
/*level:
 * 0:set SDIO2 CLK/CMD/D0~3/PWR/RSTN pinmux
 * 1:set SDIO2 CLK/CMD/D0~3 pinmux
 */
int set_sdio2_pinmux(int level);
/*level:
 * 0:set GMAC TXER/RXER/COL/CRS/TXCLKI/RXCLKI/RXD0~3/TXD0~3/RXDV/TX_EN/MDIO/MDC pinmux
 * 1:set GMAC TXCLKI/RXCLKI/RXD0~3/TXD0~3/RXDV/TX_EN/MDIO/MDC pinmux
 * 2:set GMAC TXCLKO/RXCLKI/RXD0~7/TXD0~7/RXDV/TX_EN/MDIO/MDC pinmux
 */
int set_gmac_pinmux(int level);
/*level:
 * 0:set LCDC CLK/HSYNC/VSYNC/DATAEN/R0~7/G0~7/B0~7
 * 1:set LCDC CLK/HSYNC/VSYNC/DATAEN/R0~7/G2~7/B2~7
 */
int set_lcdc_pinmux(int level);
/*level
 * 0:set UART0 RX/TX/CTS/RTS
 * 1:set UART0 RX/TX
 */
int set_uart0_pinmux(int level);
int set_uart1_pinmux();
/*level
 * 0:set UART2 RX/TX/CTS/RTS
 * 1:set UART2 RX/TX
 */
int set_uart2_pinmux(int level);
/*level
 * 0:set UART3 RX/TX/CTS/RTS
 * 1:set UART3 RX/TX
 */
int set_uart3_pinmux(int level);
int set_uart4_pinmux();
int set_uart5_pinmux();
int set_uart6_pinmux();
int set_uart7_pinmux();
int set_i2c0_pinmux();
int set_i2c1_pinmux();
int set_i2c2_pinmux();
int set_i2c3_pinmux();
int set_i2c4_pinmux();
int set_i2c5_pinmux();
int set_i2c6_pinmux();
int set_i2c_wo_pinmux();
/*level
 * 0:set PWM0 A/B/TZ
 * 1:set PWM0 A/B
 */
int set_pwm0_pinmux(int level);
/*level
 * 0:set PWM1 A/B/TZ
 * 1:set PWM1 A/B
 */
int set_pwm1_pinmux(int level);
/*level
 * 0:set PWM2 A/B/TZ
 * 1:set PWM2 A/B
 */
int set_pwm2_pinmux(int level);
/*level
 * 0:set PWM3 A/B/TZ
 * 1:set PWM3 A/B
 */
int set_pwm3_pinmux(int level);
/*level
 * 0:set PWM4 A/B/TZ
 * 1:set PWM4 A/B
 */
int set_pwm4_pinmux(int level);
/*level
 * 0:set PWM5 A/B/TZ
 * 1:set PWM5 A/B
 */
int set_pwm5_pinmux(int level);
/*level
 * 0:set PWM6 A/B/TZ
 * 1:set PWM6 A/B
 */
int set_pwm6_pinmux(int level);
int set_pwm7_pinmux();
int set_pdm0_pinmux();
int set_pdm1_pinmux();
int set_pdm2_pinmux();
int set_pdm3_pinmux();
int set_pdm4_pinmux();
int set_pdm5_pinmux();
/*level
 * 0:set I2S0 MCLK/LRCK/SCLK/D0~D3
 * 1:set I2S0 MCLK/LRCK/SCLK/D0~D1
 */
int set_i2s0_pinmux(int level);
int set_i2s1_pinmux();
int set_i2s2_pinmux();
int set_i2s3_pinmux();
int set_bootspi_pinmux();
int set_spi0_pinmux();
int set_spi1_pinmux();
int set_spi2_pinmux();
int set_spi3_pinmux();
int set_riscv0jtag_pinmux();
int set_riscv1jtag_pinmux();
int set_efuse_pinmux();
int set_gpio_pinmux(unsigned int gpio_num);
int set_debug_pinmux(unsigned int debug_num);

int set_mpll_freq(unsigned int fbdiv,unsigned int postdiv2,unsigned int postdiv1,unsigned int refdiv,unsigned int frac);

int set_fpll_freq(unsigned int fbdiv,unsigned int postdiv2,unsigned int postdiv1,unsigned int refdiv,unsigned int frac);

int set_npll_freq(unsigned int fbdiv,unsigned int postdiv2,unsigned int postdiv1,unsigned int refdiv,unsigned int frac);

int set_vpll_freq(unsigned int fbdiv,unsigned int postdiv2,unsigned int postdiv1,unsigned int refdiv,unsigned int frac);

int set_dpll_freq(unsigned int fbdiv,unsigned int postdiv2,unsigned int postdiv1,unsigned int refdiv,unsigned int frac);

int set_mpll_freq_fast(unsigned int fbdiv,unsigned int frac);

int set_fpll_freq_fast(unsigned int fbdiv,unsigned int frac);

int set_npll_freq_fast(unsigned int fbdiv,unsigned int frac);

int set_vpll_freq_fast(unsigned int fbdiv,unsigned int frac);

int set_dpll_freq_fast(unsigned int fbdiv,unsigned int frac);

int set_clk_cfg(unsigned int div,unsigned int sel);
int set_clk_top_fab1(unsigned int div);
int set_clk_gpio_db(unsigned int div);
int set_clk_adc(unsigned int div);
int set_clk_pmu(unsigned int div);
int set_clk_gtimer(unsigned int div);
int set_clk_sensor0(unsigned int div);
int set_clk_dphy_rx_esc(unsigned int div);
int set_clk_dphy_cfg(unsigned int div);
int set_clk_dphy_bist(unsigned int div);
int set_clk_dphy_rx_ref(unsigned int div,unsigned int sel);
int set_clk_dphy_tx_ref(unsigned int div,unsigned int sel);
int set_clk_lp_axi(unsigned int div,unsigned int sel);
int set_clk_i2s_lp(unsigned int div);
int set_clk_uart_lp(unsigned int div);
int set_clk_i2c_lp(unsigned int div);
int set_clk_pdm_lp(unsigned int div);
int set_clk_timer0(unsigned int div);
int set_clk_cpu_core0(unsigned int div,unsigned int sel);
int set_clk_cpu_core1(unsigned int div,unsigned int sel);
int set_clk_cpu_core2(unsigned int div,unsigned int sel);
int set_clk_cpu_core3(unsigned int div,unsigned int sel);
int set_clk_cpu_ace(unsigned int div,unsigned int sel);
int set_clk_cpu_peri(unsigned int div,unsigned int sel);
int set_clk_cpu_atb(unsigned int div,unsigned int sel);
int set_clk_cpu_gic(unsigned int div,unsigned int sel);
int set_clk_ap_ddr(unsigned int div,unsigned int sel);
int set_clk_timer1(unsigned int div);
int set_clk_ddrctrl0(unsigned int div);
int set_clk_ddrctrl1(unsigned int div);
int set_clk_peri0_ddr(unsigned int div);
int set_clk_bootspi(unsigned int div,unsigned int sel);
int set_clk_pwm_peri0(unsigned int div);
int set_clk_peri1_ddr(unsigned int div);
int set_clk_spi(unsigned int div,unsigned int sel);
int set_clk_spi_slv(unsigned int div,unsigned int sel);
int set_clk_usbctrl_suspend(unsigned int div);
int set_clk_usbphy_ref(unsigned int div,unsigned int sel);
int set_clk_usbctrl_ref(unsigned int div,unsigned int sel);
int set_clk_spacc(unsigned int div);
int set_clk_efuse(unsigned int div);
int set_clk_emmc0_trf(unsigned int div,unsigned int sel);
int set_clk_emmc0_lbt(unsigned int div);
int set_clk_emmc0_base(unsigned int div);
int set_clk_emmc0_tm(unsigned int div);
int set_clk_emmc0_cqet(unsigned int div);
int set_clk_emmc1_core(unsigned int div,unsigned int sel);
int set_clk_sdio2_core(unsigned int div,unsigned int sel);
int set_clk_gmac_mgt(unsigned int div);
int set_clk_vpu(unsigned int div,unsigned int sel);
int set_clk_gpu(unsigned int div,unsigned int sel);
int set_clk_dc_core(unsigned int div);
int set_clk_dc_pixel1(unsigned int div);
int set_clk_isp_idi(unsigned int div);
int set_clk_image_axi(unsigned int div);
int set_clk_isp_axi(unsigned int div);
int set_clk_isp_core(unsigned int div);
int set_clk_isp_mcm(unsigned int div);
int set_clk_dwp_vse(unsigned int div);
int set_clk_dwp_dwe(unsigned int div);
int set_clk_dwp_axi(unsigned int div);
int set_clk_npu0(unsigned int div,unsigned int sel);
int set_clk_npu0_sym(unsigned int div,unsigned int sel);
int set_clk_clci_ac_rx(unsigned int div);
int set_clk_clci_ac_400(unsigned int div);
int set_clk_clci_ac_485(unsigned int div);
int set_clk_clci_repair(unsigned int div);

int hwide_set_clk_cfg(unsigned int div,unsigned int sel);
int hwide_set_clk_top_fab1(unsigned int div);
int hwide_set_clk_gpio_db(unsigned int div);
int hwide_set_clk_adc(unsigned int div);
int hwide_set_clk_pmu(unsigned int div);
int hwide_set_clk_gtimer(unsigned int div);
int hwide_set_clk_sensor0(unsigned int div);
int hwide_set_clk_dphy_rx_esc(unsigned int div);
int hwide_set_clk_dphy_cfg(unsigned int div);
int hwide_set_clk_dphy_bist(unsigned int div);
int hwide_set_clk_dphy_rx_ref(unsigned int div,unsigned int sel);
int hwide_set_clk_dphy_tx_ref(unsigned int div,unsigned int sel);
int hwide_set_clk_lp_axi(unsigned int div,unsigned int sel);
int hwide_set_clk_i2s_lp(unsigned int div);
int hwide_set_clk_uart_lp(unsigned int div);
int hwide_set_clk_i2c_lp(unsigned int div);
int hwide_set_clk_pdm_lp(unsigned int div);
int hwide_set_clk_timer0(unsigned int div);
int hwide_set_clk_cpu_core0(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_core1(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_core2(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_core3(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_ace(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_peri(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_atb(unsigned int div,unsigned int sel);
int hwide_set_clk_cpu_gic(unsigned int div,unsigned int sel);
int hwide_set_clk_ap_ddr(unsigned int div,unsigned int sel);
int hwide_set_clk_timer1(unsigned int div);
int hwide_set_clk_ddrctrl0(unsigned int div);
int hwide_set_clk_ddrctrl1(unsigned int div);
int hwide_set_clk_peri0_ddr(unsigned int div);
int hwide_set_clk_bootspi(unsigned int div,unsigned int sel);
int hwide_set_clk_pwm_peri0(unsigned int div);
int hwide_set_clk_peri1_ddr(unsigned int div);
int hwide_set_clk_spi(unsigned int div,unsigned int sel);
int hwide_set_clk_spi_slv(unsigned int div,unsigned int sel);
int hwide_set_clk_usbctrl_suspend(unsigned int div);
int hwide_set_clk_usbphy_ref(unsigned int div,unsigned int sel);
int hwide_set_clk_usbctrl_ref(unsigned int div,unsigned int sel);
int hwide_set_clk_spacc(unsigned int div);
int hwide_set_clk_efuse(unsigned int div);
int hwide_set_clk_emmc0_trf(unsigned int div,unsigned int sel);
int hwide_set_clk_emmc0_lbt(unsigned int div);
int hwide_set_clk_emmc0_base(unsigned int div);
int hwide_set_clk_emmc0_tm(unsigned int div);
int hwide_set_clk_emmc0_cqet(unsigned int div);
int hwide_set_clk_emmc1_core(unsigned int div,unsigned int sel);
int hwide_set_clk_sdio2_core(unsigned int div,unsigned int sel);
int hwide_set_clk_gmac_mgt(unsigned int div);
int hwide_set_clk_vpu(unsigned int div,unsigned int sel);
int hwide_set_clk_gpu(unsigned int div,unsigned int sel);
int hwide_set_clk_dc_core(unsigned int div);
int hwide_set_clk_dc_pixel1(unsigned int div);
int hwide_set_clk_isp_idi(unsigned int div);
int hwide_set_clk_image_axi(unsigned int div);
int hwide_set_clk_isp_axi(unsigned int div);
int hwide_set_clk_isp_core(unsigned int div);
int hwide_set_clk_isp_mcm(unsigned int div);
int hwide_set_clk_dwp_vse(unsigned int div);
int hwide_set_clk_dwp_dwe(unsigned int div);
int hwide_set_clk_dwp_axi(unsigned int div);
int hwide_set_clk_npu0(unsigned int div,unsigned int sel);
int hwide_set_clk_npu0_sym(unsigned int div,unsigned int sel);
int hwide_set_clk_clci_ac_rx(unsigned int div);
int hwide_set_clk_clci_ac_400(unsigned int div);
int hwide_set_clk_clci_ac_485(unsigned int div);
int hwide_set_clk_clci_repair(unsigned int div);

int div_set_clk_cfg(unsigned int div,unsigned int sel);
int div_set_clk_top_fab1(unsigned int div);
int div_set_clk_gpio_db(unsigned int div);
int div_set_clk_adc(unsigned int div);
int div_set_clk_pmu(unsigned int div);
int div_set_clk_gtimer(unsigned int div);
int div_set_clk_sensor0(unsigned int div);
int div_set_clk_dphy_rx_esc(unsigned int div);
int div_set_clk_dphy_cfg(unsigned int div);
int div_set_clk_dphy_bist(unsigned int div);
int div_set_clk_dphy_rx_ref(unsigned int div,unsigned int sel);
int div_set_clk_dphy_tx_ref(unsigned int div,unsigned int sel);
int div_set_clk_lp_axi(unsigned int div,unsigned int sel);
int div_set_clk_i2s_lp(unsigned int div);
int div_set_clk_uart_lp(unsigned int div);
int div_set_clk_i2c_lp(unsigned int div);
int div_set_clk_pdm_lp(unsigned int div);
int div_set_clk_timer0(unsigned int div);
int div_set_clk_cpu_core0(unsigned int div,unsigned int sel);
int div_set_clk_cpu_core1(unsigned int div,unsigned int sel);
int div_set_clk_cpu_core2(unsigned int div,unsigned int sel);
int div_set_clk_cpu_core3(unsigned int div,unsigned int sel);
int div_set_clk_cpu_ace(unsigned int div,unsigned int sel);
int div_set_clk_cpu_peri(unsigned int div,unsigned int sel);
int div_set_clk_cpu_atb(unsigned int div,unsigned int sel);
int div_set_clk_cpu_gic(unsigned int div,unsigned int sel);
int div_set_clk_ap_ddr(unsigned int div,unsigned int sel);
int div_set_clk_timer1(unsigned int div);
int div_set_clk_ddrctrl0(unsigned int div);
int div_set_clk_ddrctrl1(unsigned int div);
int div_set_clk_peri0_ddr(unsigned int div);
int div_set_clk_bootspi(unsigned int div,unsigned int sel);
int div_set_clk_pwm_peri0(unsigned int div);
int div_set_clk_peri1_ddr(unsigned int div);
int div_set_clk_spi(unsigned int div,unsigned int sel);
int div_set_clk_spi_slv(unsigned int div,unsigned int sel);
int div_set_clk_usbctrl_suspend(unsigned int div);
int div_set_clk_usbphy_ref(unsigned int div,unsigned int sel);
int div_set_clk_usbctrl_ref(unsigned int div,unsigned int sel);
int div_set_clk_spacc(unsigned int div);
int div_set_clk_efuse(unsigned int div);
int div_set_clk_emmc0_trf(unsigned int div,unsigned int sel);
int div_set_clk_emmc0_lbt(unsigned int div);
int div_set_clk_emmc0_base(unsigned int div);
int div_set_clk_emmc0_tm(unsigned int div);
int div_set_clk_emmc0_cqet(unsigned int div);
int div_set_clk_emmc1_core(unsigned int div,unsigned int sel);
int div_set_clk_sdio2_core(unsigned int div,unsigned int sel);
int div_set_clk_gmac_mgt(unsigned int div);
int div_set_clk_vpu(unsigned int div,unsigned int sel);
int div_set_clk_gpu(unsigned int div,unsigned int sel);
int div_set_clk_dc_core(unsigned int div);
int div_set_clk_dc_pixel1(unsigned int div);
int div_set_clk_isp_idi(unsigned int div);
int div_set_clk_image_axi(unsigned int div);
int div_set_clk_isp_axi(unsigned int div);
int div_set_clk_isp_core(unsigned int div);
int div_set_clk_isp_mcm(unsigned int div);
int div_set_clk_dwp_vse(unsigned int div);
int div_set_clk_dwp_dwe(unsigned int div);
int div_set_clk_dwp_axi(unsigned int div);
int div_set_clk_npu0(unsigned int div,unsigned int sel);
int div_set_clk_npu0_sym(unsigned int div,unsigned int sel);
int div_set_clk_clci_ac_rx(unsigned int div);
int div_set_clk_clci_ac_400(unsigned int div);
int div_set_clk_clci_ac_485(unsigned int div);
int div_set_clk_clci_repair(unsigned int div);

int set_clk_sensor1(unsigned int div,unsigned int sel);
int set_clk_sensor2(unsigned int div,unsigned int sel);
int set_clk_sensor3(unsigned int div,unsigned int sel);
int set_clk_clci_cfg(unsigned int div,unsigned int sel);
int set_clk_lp_cfg(unsigned int div,unsigned int sel);
int set_clk_ap_cfg(unsigned int div,unsigned int sel);
int set_clk_gtimer_ap(unsigned int div,unsigned int sel);
int set_clk_ddr0_cfg(unsigned int div,unsigned int sel);
int set_clk_ddr1_cfg(unsigned int div,unsigned int sel);
int set_clk_peri0_cfg(unsigned int div,unsigned int sel);
int set_clk_gpio_db_peri0(unsigned int div,unsigned int sel);
int set_clk_i2c_peri0(unsigned int div,unsigned int sel);
int set_clk_uart_peri0(unsigned int div,unsigned int sel);
int set_clk_i2s_peri0(unsigned int div,unsigned int sel);
int set_clk_pdm_peri0(unsigned int div,unsigned int sel);
int set_clk_peri1_cfg(unsigned int div,unsigned int sel);
int set_clk_uart_peri1(unsigned int div,unsigned int sel);
int set_clk_i2c_peri1(unsigned int div,unsigned int sel);
int set_clk_i2s_peri1(unsigned int div,unsigned int sel);
int set_clk_pwm_peri1(unsigned int div,unsigned int sel);
int set_clk_vpu_cfg(unsigned int div,unsigned int sel);
int set_clk_gtimer_vpu(unsigned int div,unsigned int sel);
int set_clk_gtimer_gpu(unsigned int div,unsigned int sel);
int set_clk_gpu_cfg(unsigned int div,unsigned int sel);
int set_clk_image_cfg(unsigned int div,unsigned int sel);
int set_clk_npu0_cfg(unsigned int div,unsigned int sel);
int set_clk_npu1(unsigned int div,unsigned int sel);
int set_clk_npu1_sym(unsigned int div,unsigned int sel);
int set_clk_npu1_cfg(unsigned int div,unsigned int sel);

#endif 
