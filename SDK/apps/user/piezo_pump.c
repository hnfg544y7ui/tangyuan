#include "piezo_pump.h"
#include "asm/mcpwm.h"

static int g_pwm_pb10_id = -1;
static int g_pwm_pb9_id = -1;
static int g_pwm_pa1_id = -1;
static int g_pwm_pa0_id = -1;

/**
 * @brief Piezoelectric pump control task.
 * @param p Task parameter (unused).
 */
static void piezo_pump_task(void *p)
{
    printf("Piezo pump task started\n");
    
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

    cfg.ch = MCPWM_CH0;
    cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
    cfg.frequency = 1000;
    cfg.duty = 0;
    cfg.h_pin = IO_PORTB_10;
    cfg.l_pin = -1;
    cfg.complementary_en = 0;
    cfg.detect_port = -1;
    cfg.irq_cb = NULL;
    cfg.irq_priority = 1;
    g_pwm_pb10_id = mcpwm_init(&cfg);

    cfg.ch = MCPWM_CH1;
    cfg.h_pin = IO_PORTB_09;
    g_pwm_pb9_id = mcpwm_init(&cfg);

    cfg.ch = MCPWM_CH0 + 2;
    cfg.h_pin = IO_PORTA_01;
    g_pwm_pa1_id = mcpwm_init(&cfg);

    cfg.ch = MCPWM_CH0 + 3;
    cfg.h_pin = IO_PORTA_00;
    g_pwm_pa0_id = mcpwm_init(&cfg);

    mcpwm_start(g_pwm_pb10_id);
    mcpwm_start(g_pwm_pb9_id);
    mcpwm_start(g_pwm_pa1_id);
    mcpwm_start(g_pwm_pa0_id);

    printf("Piezo pump initialized (PB10/PB9/PA1/PA0)\n");

    int ret = os_task_create(piezo_pump_task,
                             NULL,
                             3,
                             512,
                             0,
                             "pump_task");
    
    return ret;
}

/**
 * @brief Set pump motor PWM duty cycle with direction support.
 * @param motor_id Motor ID: 0=group1(PB10/PB9), 1=group2(PA1/PA0).
 * @param duty Duty cycle range -10000~10000.
 *             Positive: first pin outputs PWM, second pin outputs 0.
 *             Negative: second pin outputs PWM, first pin outputs 0.
 *             Zero: both pins output 0 (brake).
 */
void piezo_pump_set_duty(u8 motor_id, s16 duty)
{
    if (duty > 10000) {
        duty = 10000;
    } else if (duty < -10000) {
        duty = -10000;
    }

    u16 pwm_duty = (duty >= 0) ? duty : (-duty);
    int pin1_id, pin2_id;

    switch (motor_id) {
        case 0:
            pin1_id = g_pwm_pb10_id;
            pin2_id = g_pwm_pb9_id;
            break;
        case 1:
            pin1_id = g_pwm_pa1_id;
            pin2_id = g_pwm_pa0_id;
            break;
        default:
            printf("Invalid motor_id: %d\n", motor_id);
            return;
    }

    if (duty > 0) {
        mcpwm_set_duty(pin1_id, pwm_duty);
        mcpwm_set_duty(pin2_id, 0);
    } else if (duty < 0) {
        mcpwm_set_duty(pin1_id, 0);
        mcpwm_set_duty(pin2_id, pwm_duty);
    } else {
        mcpwm_set_duty(pin1_id, 0);
        mcpwm_set_duty(pin2_id, 0);
    }
}
