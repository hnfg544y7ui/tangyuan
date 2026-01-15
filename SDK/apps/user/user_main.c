#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"
#include "btstack/third_party/rcsp/btstack_rcsp_user.h"
#include "btstack/le/att.h"
#include "fat_nor/cfg_private.h"
#include "stepper_motor.h"
#include "piezo_pump.h"

#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

static void user_gpio_init(void)
{
	gpio_set_mode(PORTA, PORT_PIN_2, PORT_INPUT_PULLUP_10K);//PA2 audio control
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

/**
 * @brief Read or write music file from/to flash storage.
 * @param write_data Data buffer to write, NULL for read-only operation.
 * @param data_len Length of data to write.
 * @param read_buf Buffer to store read data, NULL for write-only operation.
 * @param read_len Length of data to read.
 * @return 0 if success, negative value on error.
 */
int user_music_file_rw(u8 *write_data, u32 data_len, u8 *read_buf, u32 read_len)
{
	int ret = 0;
	RESFILE *fp = NULL;
	char path[64] = "flash/APP/FATFSI/";
	char file_path[64] = "flash/APP/FATFSI/music0.mp3";

	ret = cfg_private_init(10, path);
	if (ret != CFG_PRIVATE_OK) {
		printf("[USER] Init failed: %d\n", ret);
		return -1;
	}
	
	if (write_data && data_len > 0) {

		fp = cfg_private_open_by_maxsize(file_path, "w+", 4 * 1024);
		if (!fp) {
			printf("[USER] Failed to open file for writing\n");
			cfg_private_uninit();
			return -2;
		}
		
		ret = cfg_private_write(fp, write_data, data_len);
		if (ret < 0) {
			printf("[USER] Write failed: %d\n", ret);
			cfg_private_close(fp);
			cfg_private_uninit();
			return -3;
		}

		cfg_private_close(fp);
	}
	
	if (read_buf && read_len > 0) {
		
		fp = cfg_private_open_by_maxsize(file_path, "r", 4 * 1024);
		if (!fp) {
			printf("[USER] File not found or open failed\n");
			cfg_private_uninit();
			return -4;
		}
		
		ret = cfg_private_read(fp, read_buf, read_len);
		if (ret < 0) {
			printf("[USER] Read failed: %d\n", ret);
			cfg_private_close(fp);
			cfg_private_uninit();
			return -5;
		}

		cfg_private_close(fp);
	}
	
	cfg_private_uninit();
	
	return 0;
}

void user_init(void)
{
	piezo_pump_init();
	stepper_motor_init();
	user_gpio_init();
	os_task_create(user_blink_task,
				   NULL,
				   2,
				   256,
				   0,
				   "user_blink");
}
