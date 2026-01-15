#ifndef __PIEZO_PUMP_H__
#define __PIEZO_PUMP_H__

#include "system/includes.h"

/**
 * @brief Initialize piezoelectric pump PWM channels and task.
 * @return 0 if success, negative value on error.
 */
int piezo_pump_init(void);

/**
 * @brief Set pump motor PWM duty cycle with direction support.
 * @param motor_id Motor ID: 0=group1(PB10/PB9), 1=group2(PA1/PA0).
 * @param duty Duty cycle range -10000~10000.
 *             Positive: first pin outputs PWM, second pin outputs 0.
 *             Negative: second pin outputs PWM, first pin outputs 0.
 *             Zero: both pins output 0 (brake).
 */
void piezo_pump_set_duty(u8 motor_id, s16 duty);

#endif
