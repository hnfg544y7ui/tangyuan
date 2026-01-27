#include "stepper_motor.h"
#include "gpio.h"

#define STEPPER_MOTOR_DEBUG_ENABLE  1
#if STEPPER_MOTOR_DEBUG_ENABLE
#define stepper_motor_debug(fmt, ...) printf("[STEPPER_MOTOR] "fmt, ##__VA_ARGS__)
#else
#define stepper_motor_debug(...)
#endif

#define STEPPER_PIN_A      IO_PORTB_05
#define STEPPER_PIN_B      IO_PORTB_07
#define STEPPER_PIN_C      IO_PORTB_06
#define STEPPER_PIN_D      IO_PORTB_08
#define STEPPER_DRIVER_EN  IO_PORTB_11

static struct {
    stepper_mode_t mode;
    u8 step_position;
    s32 total_steps;
    s32 remain_steps;
    s8 direction;
    u16 interval_ticks;
    u16 tick_count;
    u8 busy;
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
        if (stepper_state.busy) {
            stepper_state.tick_count++;
            if (stepper_state.tick_count >= stepper_state.interval_ticks) {
                stepper_state.tick_count = 0;
                stepper_step(stepper_state.direction);
                stepper_state.remain_steps--;
                if (stepper_state.remain_steps <= 0) {
                    stepper_state.busy = 0;
                    stepper_driver_control(0);
                }
            }
        }
        os_time_dly(1);
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
    gpio_set_mode(PORTB, PORT_PIN_11, PORT_OUTPUT_LOW);
    
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
    s32 abs_steps = (steps > 0) ? steps : (-steps);
    if (abs_steps == 0) {
        return;
    }

    stepper_state.direction = (steps > 0) ? 1 : -1;
    stepper_state.remain_steps = abs_steps;
    stepper_state.tick_count = 0;
    stepper_state.interval_ticks = delay_ms / 10;
    if (stepper_state.interval_ticks == 0) {
        stepper_state.interval_ticks = 1;
    }
    stepper_state.busy = 1;
    stepper_driver_control(1);
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
    stepper_motor_debug("Stepper mode: %s\n", mode_str[mode]);
}

/**
 * @brief Stop stepper motor and power off.
 */
void stepper_stop(void)
{
    stepper_state.busy = 0;
    stepper_state.remain_steps = 0;
    stepper_state.tick_count = 0;
    stepper_driver_control(0);
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

/**
 * @brief Get current position in steps.
 * @return Current step position.
 */
s32 stepper_get_position(void)
{
    return stepper_state.total_steps;
}

/**
 * @brief Get stepper running status.
 * @return 1 if running, 0 if idle.
 */
u8 stepper_is_running(void)
{
    return stepper_state.busy ? 1 : 0;
}

/**
 * @brief Control stepper motor driver power.
 * @param enable 1=ON, 0=OFF.
 */
void stepper_driver_control(u8 enable)
{
    gpio_write(STEPPER_DRIVER_EN, enable ? 1 : 0);
    stepper_motor_debug("Stepper driver %s\n", enable ? "ON" : "OFF");
}
