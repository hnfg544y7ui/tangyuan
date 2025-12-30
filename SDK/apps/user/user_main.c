#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"
#include "btstack/third_party/rcsp/btstack_rcsp_user.h"
#include "btstack/le/att.h"

#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

static void user_gpio_init(void)
{
	/* Configure as output low. IO_PORT_SPILT expands to (port, pin_mask). */
	// gpio_set_mode(IO_PORT_SPILT(USER_BLINK_GPIO), PORT_OUTPUT_LOW);
	// u32 gpio = IO_PORTB_00;
	// gpio_set_pull_down(gpio, 0);
	// gpio_set_pull_up(gpio, 0);
	// gpio_set_die(gpio, 1);
	// gpio_set_hd(gpio, 0);
	// gpio_set_hd0(gpio, 0);
	// gpio_set_direction(gpio, 0);
	// gpio_set_output_value(gpio, 1);
}

static void user_blink_task(void *p)
{
	u8 level = 0;
	static u32 counter = 0;
	
	while (1) {
		os_time_dly(1000);
	}
}


/**
 * @brief Handles BLE notification or indication data received from a remote device
 * 
 * @param ble_con_hdl The BLE connection handle identifying the active connection
 * @param remote_addr Pointer to the remote device's BLE address
 * @param buf Pointer to the buffer containing the received notification/indication data
 * @param len Length of the data in the buffer (in bytes)
 * @param att_handle The ATT (Attribute Protocol) handle that identifies the characteristic
 *                   from which the notification/indication was sent
 */
void bt_rcsp_custom_recieve_callback(u16 ble_con_hdl, void *remote_addr, u8 *buf, u16 len, uint16_t att_handle)
{
	printf("[BLE RX] hdl:%d, len:%d, att:0x%04x\n", ble_con_hdl, len, att_handle);
	printf("[BLE RX] Data: ");
	for(u16 i = 0; i < len; i++) {
		printf("%02X ", buf[i]);
	}
	printf("\n");
	
}

/**
 * @brief Handles BLE data reception or processing
 * 
 * @param ble_con_hdl BLE connection handle identifier
 * @param data Pointer to the data buffer to be processed
 * @param len Length of the data buffer in bytes
 */
void user_bt_send_custom_data(u16 ble_con_hdl, u8 *data, u16 len)
{	
	bt_rcsp_custom_data_send(ble_con_hdl, NULL, data, len, 0, ATT_OP_AUTO_READ_CCC);
	printf("[BLE TX] Sent %d bytes to hdl:%d\n", len, ble_con_hdl);
}

// ============================================================================

void user_init(void)
{
	
	os_task_create(user_blink_task,
				   NULL,
				   2,
				   256,
				   0,
				   "user_blink");
}
