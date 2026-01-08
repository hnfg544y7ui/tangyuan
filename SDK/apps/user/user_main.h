#ifndef USER_MAIN_H
#define USER_MAIN_H

#include "generic/typedef.h"
#include "gpio.h"

/* Default blink GPIO; override before including if needed. */
#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

void user_init(void);

void bt_rcsp_custom_recieve_callback(u16 ble_con_hdl, void *remote_addr, u8 *buf, u16 len, uint16_t att_handle);
void user_bt_send_custom_data(u16 ble_con_hdl, u8 *data, u16 len);

#endif /* USER_MAIN_H */
