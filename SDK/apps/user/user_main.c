#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"

#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

static void user_gpio_init(void)
{
	/* Configure as output low. IO_PORT_SPILT expands to (port, pin_mask). */
	gpio_set_mode(IO_PORT_SPILT(USER_BLINK_GPIO), PORT_OUTPUT_LOW);
}

static void user_blink_task(void *p)
{
	u8 level = 0;

	user_gpio_init();

	while (1) {
		level = !level;
		gpio_write(USER_BLINK_GPIO, level);
		os_time_dly(100);
	}
}

void user_init(void)
{
	os_task_create(user_blink_task,
				   NULL,
				   6,
				   256,
				   0,
				   "user_blink");
}
