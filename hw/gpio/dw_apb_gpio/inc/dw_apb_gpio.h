#ifndef __APE1210_GPIO_H__
#define __APE1210_GPIO_H__
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum { GROUP_GPIO0 = 0, GROUP_GPIO1 = 1, GROUP_GPIO2 = 2 } gpio_group_t;

typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET = 1 } gpio_pin_state_t;

typedef enum { Software_Mode = 0, Hardware_Mode = 1 } gpio_control_mode_t;

typedef enum {
	GPIO_Output_Mode = 0,
	GPIO_Input_Mode = 1,
	GPIO_Rising_Int_Mode = 2,
	GPIO_Falling_Int_Mode = 3,
	GPIO_Low_Int_Mode = 4,
	GPIO_High_Int_Mode = 5,
	GPIO_Edge_Int_Mode = 6
} gpio_mode_t;
typedef struct _gpio_init_config {
	// bool is_software_mode;
	// bool is_output_mode;
	gpio_control_mode_t gpio_control_mode;
	gpio_mode_t gpio_mode;
	gpio_group_t group;
	uint8_t pin;
} gpio_init_config_t;

/**
 * @brief 初始化gpio,禁止在中断中使用
 * @param gpio_init_config 初始化结构体对象地址
 */
void gpio_init(gpio_init_config_t const *const gpio_init_config);

/**
 * @brief 清除pin的中断状态
 * @param group 设置gpio组
 * @param pin   设置pin
 */
void gpio_clear_interrput(gpio_group_t group, uint8_t pin);

/**
 * @brief 获得gpio引脚的电平状态
 * @param group 设置gpio组
 * @param pin   设置pin
 * @return GPIO_PIN_SET or GPIO_PIN_RESET
 */
gpio_pin_state_t gpio_read_pin(gpio_group_t group, uint8_t pin);

/**
 * @brief 输出gpio引脚的电平状态,禁止在中断中使用
 * @param group 设置gpio组
 * @param pin   设置pin
 * @param pin_state GPIO_PIN_SET or GPIO_PIN_RESET
 */
void gpio_write_pin(gpio_group_t group, uint8_t pin, gpio_pin_state_t pin_state);

/**
 * @brief 使用gpio之前务必先调用该函数，初始化mutexs(只需要初始化一次)
 */
void gpio_mutexs_init(void);

#ifdef __cplusplus
    }
#endif

#endif