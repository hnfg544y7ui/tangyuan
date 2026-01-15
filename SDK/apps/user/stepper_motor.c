#include "stepper_motor.h"
#include "gpio.h"

#define STEPPER_PIN_A  IO_PORTB_05
#define STEPPER_PIN_B  IO_PORTB_06
#define STEPPER_PIN_C  IO_PORTB_07
#define STEPPER_PIN_D  IO_PORTB_08

static struct {
    stepper_mode_t mode;
    u8 step_position;
    s32 total_steps;
} stepper_state = {STEPPER_MODE_HALF, 0, 0};

static const u8 step_seq_single[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
};

static const u8 step_seq_double[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 0, 1},
};

static const u8 step_seq_half[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

/**
 * @brief Set stepper motor phase outputs.
 * @param step_index Step index (0-3 for single/double, 0-7 for half).
 */
static void stepper_set_phase(u8 step_index)
{
    const u8 *phase;
    u8 max_steps;
    
    switch (stepper_state.mode) {
        case STEPPER_MODE_SINGLE:
            phase = step_seq_single[step_index % 4];
            max_steps = 4;
            break;
        case STEPPER_MODE_DOUBLE:
            phase = step_seq_double[step_index % 4];
            max_steps = 4;
            break;
        case STEPPER_MODE_HALF:
        default:
            phase = step_seq_half[step_index % 8];
            max_steps = 8;
            break;
    }
    
    gpio_write(STEPPER_PIN_A, phase[0]);
    gpio_write(STEPPER_PIN_B, phase[1]);
    gpio_write(STEPPER_PIN_C, phase[2]);
    gpio_write(STEPPER_PIN_D, phase[3]);
    
    stepper_state.step_position = step_index % max_steps;
}

/**
 * @brief Stepper motor control task.
 * @param p Task parameter (unused).
 */
static void stepper_motor_task(void *p)
{
    while (1) {
        os_time_dly(100);
    }
}

/**
 * @brief Initialize stepper motor GPIO and task.
 * @return 0 if success, negative value on error.
 */
int stepper_motor_init(void)
{
    gpio_set_mode(PORTB, PORT_PIN_5, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTB, PORT_PIN_6, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTB, PORT_PIN_7, PORT_OUTPUT_LOW);
    gpio_set_mode(PORTB, PORT_PIN_8, PORT_OUTPUT_LOW);
    
    stepper_state.mode = STEPPER_MODE_HALF;
    stepper_state.step_position = 0;
    stepper_state.total_steps = 0;
    
    stepper_set_phase(0);
    
    int ret = os_task_create(stepper_motor_task,
                             NULL,
                             3,
                             512,
                             0,
                             "stepper_task");
    
    return ret;
}

/**
 * @brief Single step control for stepper motor.
 * @param direction 1=forward, -1=reverse.
 */
void stepper_step(s8 direction)
{
    u8 max_steps = (stepper_state.mode == STEPPER_MODE_HALF) ? 8 : 4;
    
    if (direction > 0) {
        stepper_state.step_position = (stepper_state.step_position + 1) % max_steps;
        stepper_state.total_steps++;
    } else {
        stepper_state.step_position = (stepper_state.step_position + max_steps - 1) % max_steps;
        stepper_state.total_steps--;
    }
    
    stepper_set_phase(stepper_state.step_position);
}

/**
 * @brief Move stepper motor by specified steps.
 * @param steps Step count (positive=forward, negative=reverse).
 * @param delay_ms Delay between each step in milliseconds.
 */
void stepper_move(s32 steps, u16 delay_ms)
{
    s8 direction = (steps > 0) ? 1 : -1;
    u32 abs_steps = (steps > 0) ? steps : (-steps);
    
    for (u32 i = 0; i < abs_steps; i++) {
        stepper_step(direction);
        os_time_dly(delay_ms / 10);
    }
}

/**
 * @brief Set stepper motor mode.
 * @param mode STEPPER_MODE_SINGLE / DOUBLE / HALF.
 */
void stepper_set_mode(stepper_mode_t mode)
{
    stepper_state.mode = mode;
    stepper_state.step_position = 0;
    stepper_set_phase(0);
    
    const char *mode_str[] = {"SINGLE", "DOUBLE", "HALF"};
    printf("Stepper mode: %s\n", mode_str[mode]);
}

/**
 * @brief Stop stepper motor and power off.
 */
void stepper_stop(void)
{
    gpio_write(STEPPER_PIN_A, 0);
    gpio_write(STEPPER_PIN_B, 0);
    gpio_write(STEPPER_PIN_C, 0);
    gpio_write(STEPPER_PIN_D, 0);
}

/**
 * @brief Get total step count.
 * @return Total steps accumulated.
 */
s32 stepper_get_total_steps(void)
{
    return stepper_state.total_steps;
}
