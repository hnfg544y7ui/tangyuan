#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#include "system/includes.h"

// Gear reduction ratio
#define STEPPER_REDUCTION_RATIO  15

typedef enum {
    STEPPER_MODE_SINGLE = 0,
    STEPPER_MODE_DOUBLE = 1,
    STEPPER_MODE_HALF = 2,
} stepper_mode_t;

/**
 * @brief Initialize stepper motor GPIO and task.
 * @return 0 if success, negative value on error.
 */
int stepper_motor_init(void);

/**
 * @brief Single step control for stepper motor.
 * @param direction 1=forward, -1=reverse.
 */
void stepper_step(s8 direction);

/**
 * @brief Move stepper motor by specified steps.
 * @param steps Step count (positive=forward, negative=reverse).
 * @param delay_ms Delay between each step in milliseconds.
 */
void stepper_move(s32 steps, u16 delay_ms);

/**
 * @brief Set stepper motor mode.
 * @param mode STEPPER_MODE_SINGLE / DOUBLE / HALF.
 */
void stepper_set_mode(stepper_mode_t mode);

/**
 * @brief Stop stepper motor and power off.
 */
void stepper_stop(void);

/**
 * @brief Get total step count.
 * @return Total steps accumulated.
 */
s32 stepper_get_total_steps(void);

/**
 * @brief Control stepper motor driver power.
 * @param enable 1=ON, 0=OFF.
 */
void stepper_driver_control(u8 enable);

#endif
