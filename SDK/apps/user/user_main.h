#ifndef USER_MAIN_H
#define USER_MAIN_H

#include "generic/typedef.h"
#include "gpio.h"

/* Default blink GPIO; override before including if needed. */
#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

void user_init(void);

#endif /* USER_MAIN_H */
