#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#include "system/includes.h"

// RGB and White LED IDs (PWM controlled)
typedef enum {
    LED_RED = 0,    // PC4
    LED_GREEN = 1,  // PC5
    LED_BLUE = 2,   // PC0
    LED_WHITE = 3,  // PA6
} led_pwm_t;

// GPIO LED IDs (High/Low controlled)
typedef enum {
    LED_GPIO_1 = 0, // PC1
    LED_GPIO_2 = 1, // PC2
    LED_GPIO_3 = 2, // PC6
    LED_GPIO_4 = 3, // PA7
} led_gpio_t;

/**
 * @brief Initialize LED control module.
 * @return 0 if success, negative value on error.
 */
int led_control_init(void);

/**
 * @brief Set PWM LED brightness.
 * @param led LED ID (LED_RED/GREEN/BLUE/WHITE).
 * @param frequency PWM frequency in Hz (e.g., 1000 for 1kHz).
 * @param duty Duty cycle 0-10000 (0%~100%).
 */
void led_pwm_set(led_pwm_t led, u32 frequency, u16 duty);

/**
 * @brief Set RGB and White LED brightness.
 * @param t_r Red brightness 0-10000.
 * @param t_g Green brightness 0-10000.
 * @param t_b Blue brightness 0-10000.
 * @param t_w White brightness 0-10000.
 */
void led_rgb_set(u16 t_r, u16 t_g, u16 t_b, u16 t_w);

/**
 * @brief Set GPIO LED state.
 * @param led LED ID (LED_GPIO_1/2/3/4).
 * @param state 1=ON, 0=OFF.
 */
void led_gpio_set(led_gpio_t led, u8 state);

/**
 * @brief Set all GPIO LEDs state.
 * @param state 1=ON, 0=OFF.
 */
void led_gpio_set_all(u8 state);

#endif
