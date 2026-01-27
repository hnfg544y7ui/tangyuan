#include "piezo_pump.h"
#include "asm/mcpwm.h"
#include "gpio.h"

#define PIEZO_PUMP_DEBUG_ENABLE  0
#if PIEZO_PUMP_DEBUG_ENABLE
#define piezo_pump_debug(fmt, ...) printf("[PIEZO_PUMP] "fmt, ##__VA_ARGS__)
#else
#define piezo_pump_debug(...)
#endif

static int g_pwm_motor1_id = -1;
static int g_pwm_motor2_id = -1;

/**
 * @brief Piezoelectric pump control task.
 * @param p Task parameter (unused).
 */
static void piezo_pump_task(void *p)
{
    while (1) {
        os_time_dly(100);
    }
}

/**
 * @brief Initialize piezoelectric pump PWM channels and task.
 * @return 0 if success, negative value on error.
 */
int piezo_pump_init(void)
{
    struct mcpwm_config cfg;

    // Configure PC3 as voltage enable pin
    gpio_set_mode(PORTC, PORT_PIN_3, PORT_OUTPUT_HIGH);

    // Motor 1: PB10(H)/PB9(L)
    cfg.ch = MCPWM_CH0;
    cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
    cfg.frequency = 20000;
    cfg.duty = 0;
    cfg.h_pin = IO_PORTB_10;
    cfg.l_pin = IO_PORTB_09;
    cfg.complementary_en = 1;
    cfg.detect_port = -1;
    cfg.irq_cb = NULL;
    cfg.irq_priority = 1;
    g_pwm_motor1_id = mcpwm_init(&cfg);

    // Motor 2: PA1(H)/PA0(L)
    cfg.ch = MCPWM_CH1;
    cfg.h_pin = IO_PORTA_01;
    cfg.l_pin = IO_PORTA_00;
    cfg.complementary_en = 1;
    g_pwm_motor2_id = mcpwm_init(&cfg);

    mcpwm_start(g_pwm_motor1_id);
    mcpwm_start(g_pwm_motor2_id);

    piezo_pump_debug("Piezo pump initialized (complementary mode: PB10/PB9, PA1/PA0)\n");

    int ret = os_task_create(piezo_pump_task,
                             NULL,
                             3,
                             512,
                             0,
                             "pump_task");
    
    return ret;
}

/**
 * @brief Set pump motor PWM duty cycle (complementary mode).
 * @param motor_id Motor ID: 0=motor1(PB10/PB9), 1=motor2(PA1/PA0).
 * @param frequency PWM frequency in Hz (e.g., 20000 for 20kHz).
 * @param duty Duty cycle range 0-10000 (0%-100%).
 */
static void piezo_pump_set_duty(u8 motor_id, u32 frequency, s16 duty)
{
    if (duty > 10000) {
        duty = 10000;
    } else if (duty < 0) {
        duty = 0;
    }

    int pwm_id;

    switch (motor_id) {
        case 0:
            pwm_id = g_pwm_motor1_id;
            break;
        case 1:
            pwm_id = g_pwm_motor2_id;
            break;
        default:
            piezo_pump_debug("Invalid motor_id: %d\n", motor_id);
            return;
    }

    mcpwm_set_frequency(pwm_id, MCPWM_EDGE_ALIGNED, frequency);
    mcpwm_set_duty(pwm_id, duty);
}

/**
 * @brief Start piezo pump with specified frequency.
 * @param motor_id Motor ID: 0=motor1(PB10/PB9), 1=motor2(PA1/PA0).
 * @param frequency PWM frequency in Hz (e.g., 20000 for 20kHz).
 */
void piezo_pump_run(u8 motor_id, u32 frequency)
{
    piezo_pump_set_duty(motor_id, frequency, 5000);
}

/**
 * @brief Stop piezo pump.
 * @param motor_id Motor ID: 0=motor1(PB10/PB9), 1=motor2(PA1/PA0).
 */
void piezo_pump_stop(u8 motor_id)
{
    piezo_pump_set_duty(motor_id, 20000, 0);
}
