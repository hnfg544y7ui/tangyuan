#include "led_control.h"
#include "asm/mcpwm.h"
#include "gpio.h"

#define LED_CONTROL_DEBUG_ENABLE  0
#if LED_CONTROL_DEBUG_ENABLE
#define led_control_debug(fmt, ...) printf("[LED_CONTROL] "fmt, ##__VA_ARGS__)
#else
#define led_control_debug(...)
#endif

// PWM LED pin definitions
#define LED_RED_PIN    IO_PORTC_04
#define LED_GREEN_PIN  IO_PORTC_05
#define LED_BLUE_PIN   IO_PORTC_00
#define LED_WHITE_PIN  IO_PORTA_06

// GPIO LED pin definitions
#define LED_GPIO1_PIN  IO_PORTC_01
#define LED_GPIO2_PIN  IO_PORTC_02
#define LED_GPIO3_PIN  IO_PORTC_06
#define LED_GPIO4_PIN  IO_PORTA_07

static int g_pwm_led_ids[4] = {-1, -1, -1, -1};

/**
 * @brief Initialize PWM LEDs.
 * @return 0 if success, negative value on error.
 */
static int led_pwm_init(void)
{
    struct mcpwm_config cfg;
    
    // Red LED (PC4) - MCPWM CH2
    cfg.ch = MCPWM_CH2;
    cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
    cfg.frequency = 1000;
    cfg.duty = 0;
    cfg.h_pin = LED_RED_PIN;
    cfg.l_pin = -1;
    cfg.complementary_en = 0;
    cfg.detect_port = -1;
    cfg.irq_cb = NULL;
    cfg.irq_priority = 1;
    g_pwm_led_ids[LED_RED] = mcpwm_init(&cfg);
    if (g_pwm_led_ids[LED_RED] < 0) {
        led_control_debug("LED Red PWM init failed\n");
        return -1;
    }
    
    // Green LED (PC5) - MCPWM CH3
    cfg.ch = MCPWM_CH3;
    cfg.h_pin = LED_GREEN_PIN;
    g_pwm_led_ids[LED_GREEN] = mcpwm_init(&cfg);
    if (g_pwm_led_ids[LED_GREEN] < 0) {
        led_control_debug("LED Green PWM init failed\n");
        return -2;
    }
    
    // Blue LED (PC0) - MCPWM CH4
    cfg.ch = MCPWM_CH4;
    cfg.h_pin = LED_BLUE_PIN;
    g_pwm_led_ids[LED_BLUE] = mcpwm_init(&cfg);
    if (g_pwm_led_ids[LED_BLUE] < 0) {
        led_control_debug("LED Blue PWM init failed\n");
        return -3;
    }
    
    // White LED (PA6) - MCPWM CH5
    cfg.ch = MCPWM_CH5;
    cfg.h_pin = LED_WHITE_PIN;
    g_pwm_led_ids[LED_WHITE] = mcpwm_init(&cfg);
    if (g_pwm_led_ids[LED_WHITE] < 0) {
        led_control_debug("LED White PWM init failed\n");
        return -4;
    }
    
    mcpwm_start(g_pwm_led_ids[LED_RED]);
    mcpwm_start(g_pwm_led_ids[LED_GREEN]);
    mcpwm_start(g_pwm_led_ids[LED_BLUE]);
    mcpwm_start(g_pwm_led_ids[LED_WHITE]);
    
    led_control_debug("PWM LEDs initialized (R:PC4, G:PC5, B:PC0, W:PA6)\n");
    
    return 0;
}

/**
 * @brief Initialize GPIO LEDs.
 * @return 0 if success, negative value on error.
 */
static int led_gpio_init(void)
{
    gpio_set_mode(PORTC, PORT_PIN_1, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTC, PORT_PIN_2, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTC, PORT_PIN_6, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTA, PORT_PIN_7, PORT_OUTPUT_LOW);
    
    led_control_debug("GPIO LEDs initialized (PC1, PC2, PC6, PA7)\n");
    
    return 0;
}

/**
 * @brief Initialize LED control module.
 * @return 0 if success, negative value on error.
 */
int led_control_init(void)
{
    int ret;
    
    ret = led_pwm_init();
    if (ret < 0) {
        return ret;
    }
    
    ret = led_gpio_init();
    if (ret < 0) {
        return ret;
    }
    
    led_control_debug("LED control module initialized\n");
    
    return 0;
}

/**
 * @brief Set PWM LED brightness.
 * @param led LED ID (LED_RED/GREEN/BLUE/WHITE).
 * @param frequency PWM frequency in Hz (e.g., 1000 for 1kHz).
 * @param duty Duty cycle 0~10000 (0%~100%).
 */
static void led_pwm_set(led_pwm_t led, u32 frequency, u16 duty)
{
    if (led >= 4) {
        led_control_debug("Invalid LED ID: %d\n", led);
        return;
    }
    
    if (g_pwm_led_ids[led] < 0) {
        led_control_debug("LED %d not initialized\n", led);
        return;
    }
    
    if (duty > 10000) {
        duty = 10000;
    }
    
    mcpwm_set_frequency(g_pwm_led_ids[led], MCPWM_EDGE_ALIGNED, frequency);
    mcpwm_set_duty(g_pwm_led_ids[led], duty);
}

/**
 * @brief Set RGB and White LED brightness.
 * @param t_r Red brightness 0-10000.
 * @param t_g Green brightness 0-10000.
 * @param t_b Blue brightness 0-10000.
 * @param t_w White brightness 0-10000.
 */
void led_rgb_set(u16 t_r, u16 t_g, u16 t_b, u16 t_w)
{
    led_pwm_set(LED_RED, 10000, t_r);
    led_pwm_set(LED_GREEN, 10000, t_g);
    led_pwm_set(LED_BLUE, 10000, t_b);
    led_pwm_set(LED_WHITE, 10000, t_w);
}

/**
 * @brief Set GPIO LED state.
 * @param led LED ID (LED_GPIO_1/2/3/4).
 * @param state 1=ON, 0=OFF.
 */
void led_gpio_set(led_gpio_t led, u8 state)
{
    u32 pin;
    
    switch (led) {
        case LED_GPIO_1:
            pin = LED_GPIO1_PIN;
            break;
        case LED_GPIO_2:
            pin = LED_GPIO2_PIN;
            break;
        case LED_GPIO_3:
            pin = LED_GPIO3_PIN;
            break;
        case LED_GPIO_4:
            pin = LED_GPIO4_PIN;
            break;
        default:
            led_control_debug("Invalid GPIO LED ID: %d\n", led);
            return;
    }
    
    gpio_write(pin, state ? 1 : 0);
}

/**
 * @brief Set all GPIO LEDs state.
 * @param state 1=ON, 0=OFF.
 */
void led_gpio_set_all(u8 state)
{
    gpio_write(LED_GPIO1_PIN, state ? 1 : 0);
    gpio_write(LED_GPIO2_PIN, state ? 1 : 0);
    gpio_write(LED_GPIO3_PIN, state ? 1 : 0);
    gpio_write(LED_GPIO4_PIN, state ? 1 : 0);
}
