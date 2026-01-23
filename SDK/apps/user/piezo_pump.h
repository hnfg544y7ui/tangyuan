#ifndef __PIEZO_PUMP_H__
#define __PIEZO_PUMP_H__

#include "system/includes.h"

/**
 * @brief Initialize piezoelectric pump PWM channels and task.
 * @return 0 if success, negative value on error.
 */
int piezo_pump_init(void);

/**
 * @brief Start piezo pump with specified frequency.
 * @param motor_id Motor ID: 0=motor1(PB10/PB9), 1=motor2(PA1/PA0).
 * @param frequency PWM frequency in Hz (e.g., 20000 for 20kHz).
 */
void piezo_pump_run(u8 motor_id, u32 frequency);

#endif
