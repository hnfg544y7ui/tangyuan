#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"
#include "btstack/third_party/rcsp/btstack_rcsp_user.h"
#include "btstack/le/att.h"
#include "fat_nor/cfg_private.h"
#include "stepper_motor.h"
#include "piezo_pump.h"
#include "uart_comm.h"
#include "nfc_reader.h"
#include "led_control.h"
#include "key_check.h"

#define USER_MAIN_DEBUG_ENABLE  0
#if USER_MAIN_DEBUG_ENABLE
#define user_main_debug(fmt, ...) printf("[USER_MAIN] "fmt, ##__VA_ARGS__)
#else
#define user_main_debug(...)
#endif

#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

/**
 * @brief Handle key events from touch key.
 * @param t_key_id Key ID.
 * @param t_event Key event type.
 */
static void key_event_handler(key_id_t t_key_id, key_event_t t_event)
{
	user_main_debug("[KEY EVENT] id:%d ", t_key_id);
	switch (t_event) {
		case KEY_EVENT_SHORT_PRESS:{
			user_main_debug("[KEY] Short press\n");
		}break;
		
		case KEY_EVENT_LONG_PRESS:{
			user_main_debug("[KEY] Long press\n");
		}break;
		
		case KEY_EVENT_DOUBLE_CLICK:{
			user_main_debug("[KEY] Double click\n");
		}break;
		
		case KEY_EVENT_TRIPLE_CLICK:{
			user_main_debug("[KEY] Triple click\n");
		}break;
		
		default:
			break;
	}
}

static void user_gpio_init(void)
{
	gpio_set_mode(PORTA, PORT_PIN_2, PORT_INPUT_PULLUP_10K);//PA2 audio control
	gpio_set_mode(PORTB, PORT_PIN_1, PORT_INPUT_PULLUP_10K);
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
	user_main_debug("[BLE RX] hdl:%d, len:%d, att:0x%04x\n", ble_con_hdl, len, att_handle);
	user_main_debug("[BLE RX] Data: ");
	for (u16 i = 0; i < len; i++) {
		user_main_debug("%02X ", buf[i]);
	}
	user_main_debug("\n");
	
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
	user_main_debug("[BLE TX] Sent %d bytes to hdl:%d\n", len, ble_con_hdl);
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
		user_main_debug("[USER] Init failed: %d\n", ret);
		return -1;
	}
	
	if (write_data && data_len > 0) {

		fp = cfg_private_open_by_maxsize(file_path, "w+", 4 * 1024);
		if (!fp) {
			user_main_debug("[USER] Failed to open file for writing\n");
			cfg_private_uninit();
			return -2;
		}
		
		ret = cfg_private_write(fp, write_data, data_len);
		if (ret < 0) {
			user_main_debug("[USER] Write failed: %d\n", ret);
			cfg_private_close(fp);
			cfg_private_uninit();
			return -3;
		}

		cfg_private_close(fp);
	}
	
	if (read_buf && read_len > 0) {
		
		fp = cfg_private_open_by_maxsize(file_path, "r", 4 * 1024);
		if (!fp) {
			user_main_debug("[USER] File not found or open failed\n");
			cfg_private_uninit();
			return -4;
		}
		
		ret = cfg_private_read(fp, read_buf, read_len);
		if (ret < 0) {
			user_main_debug("[USER] Read failed: %d\n", ret);
			cfg_private_close(fp);
			cfg_private_uninit();
			return -5;
		}

		cfg_private_close(fp);
	}
	
	cfg_private_uninit();
	
	return 0;
}

/**
 * @brief Write data to a file.
 * @param t_file_name File name.
 * @param t_data Pointer to data buffer.
 * @param t_len Length of data.
 * @return 0 if success, negative value on error.
 */
int user_file_write(const char *t_file_name, const u8 *t_data, u32 t_len)
{
	int ret = 0;
	RESFILE *fp = NULL;
	char path[64] = "flash/APP/FATFSI/";
	char file_path[128] = {0};

	if (t_file_name == NULL || t_data == NULL || t_len == 0) {
		return -1;
	}

	ret = cfg_private_init(10, path);
	if (ret != CFG_PRIVATE_OK) {
		return -2;
	}

	snprintf(file_path, sizeof(file_path), "%s%s", path, t_file_name);
	
	fp = cfg_private_open_by_maxsize(file_path, "w+", 4 * 1024);
	if (!fp) {
		cfg_private_uninit();
		return -3;
	}

	ret = cfg_private_write(fp, (void *)t_data, t_len);
	if (ret < 0) {
		cfg_private_close(fp);
		cfg_private_uninit();
		return -4;
	}

	cfg_private_close(fp);
	cfg_private_uninit();
	return 0;
}

/**
 * @brief Read data from a file.
 * @param t_file_name File name.
 * @param t_buf Pointer to read buffer.
 * @param t_len Length to read.
 * @return 0 if success, negative value on error.
 */
int user_file_read(const char *t_file_name, u8 *t_buf, u32 t_len)
{
	int ret = 0;
	RESFILE *fp = NULL;
	char path[64] = "flash/APP/FATFSI/";
	char file_path[128] = {0};

	if (t_file_name == NULL || t_buf == NULL || t_len == 0) {
		return -1;
	}

	ret = cfg_private_init(10, path);
	if (ret != CFG_PRIVATE_OK) {
		return -2;
	}

	snprintf(file_path, sizeof(file_path), "%s%s", path, t_file_name);

	fp = cfg_private_open_by_maxsize(file_path, "r", 4 * 1024);
	if (!fp) {
		cfg_private_uninit();
		return -3;
	}

	ret = cfg_private_read(fp, t_buf, t_len);
	if (ret < 0) {
		cfg_private_close(fp);
		cfg_private_uninit();
		return -4;
	}

	cfg_private_close(fp);
	cfg_private_uninit();
	return 0;
}

void user_init(void)
{
	piezo_pump_init();
	stepper_motor_init();
	uart_comm_init();
	nfc_reader_init();
	led_control_init();
	key_check_init(key_event_handler);
	user_gpio_init();
	os_task_create(user_blink_task,
				   NULL,
				   2,
				   256,
				   0,
				   "user_blink");
}
