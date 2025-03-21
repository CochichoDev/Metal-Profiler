#include "gpio.h"

void sel_gpio_func(u8 gpio_num, u8 gpio_func) {
    u32 reg_value = *REG_GPIO_GPFSEL(gpio_num);
    reg_value &= ~(0x7 << SHIFT_GPIO_GPFSEL(gpio_num));
    reg_value |= (gpio_func << SHIFT_GPIO_GPFSEL(gpio_num));
    *REG_GPIO_GPFSEL(gpio_num) = reg_value;
}

void set_gpio_output(u8 gpio_num) {
    *REG_GPIO_GPSET(gpio_num) |= (1 << SHIFT_GPIO_GPSET(gpio_num));
}

void clr_gpio_output(u8 gpio_num) {
    *REG_GPIO_GPCLR(gpio_num) |= (1 << SHIFT_GPIO_GPCLR(gpio_num));
}

void conf_gpio_pupd(u8 gpio_num, u8 pupd) {
    u32 reg_value = *REG_GPIO_PU_PD(gpio_num);
    reg_value &= ~(0x3 << SHIFT_GPIO_PU_PD(gpio_num));
    reg_value |= (pupd << SHIFT_GPIO_PU_PD(gpio_num));
    *REG_GPIO_PU_PD(gpio_num) = reg_value;
}
