#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"
#include "btstack/third_party/rcsp/btstack_rcsp_user.h"
#include "btstack/le/att.h"
#include "asm/mcpwm.h"
#include "fat_nor/cfg_private.h"
#include "fs/resfile.h"

// Function declarations
int user_music_file_rw(u8 *write_data, u32 data_len, u8 *read_buf, u32 read_len);

#ifndef USER_BLINK_GPIO
#define USER_BLINK_GPIO   IO_PORTB_01
#endif

static int g_pwm_pb10_id = -1;
static int g_pwm_pb9_id = -1;
static int g_pwm_pa1_id = -1;
static int g_pwm_pa0_id = -1;

/**
 * @brief Initialize PWM channels for motor control.
 * @return 0 if initialization succeeds.
 */
uint8_t pwm_init(void)
{
	struct mcpwm_config usr_mcpwm_cfg;

	usr_mcpwm_cfg.ch = MCPWM_CH0;
	usr_mcpwm_cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
	usr_mcpwm_cfg.frequency = 1000;	   // frequency Hz
	usr_mcpwm_cfg.duty = 0;			   // duty 0~10000
	usr_mcpwm_cfg.h_pin = IO_PORTB_05; // port
	usr_mcpwm_cfg.l_pin = -1;
	usr_mcpwm_cfg.complementary_en = 0;
	usr_mcpwm_cfg.detect_port = -1;
	usr_mcpwm_cfg.irq_cb = NULL;
	usr_mcpwm_cfg.irq_priority = 1;
	g_pwm_pb10_id = mcpwm_init(&usr_mcpwm_cfg);

	usr_mcpwm_cfg.ch = MCPWM_CH1;
	usr_mcpwm_cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
	usr_mcpwm_cfg.frequency = 1000;	   // frequency Hz
	usr_mcpwm_cfg.duty = 0;			   // duty 0~10000
	usr_mcpwm_cfg.h_pin = IO_PORTB_06; // port
	usr_mcpwm_cfg.l_pin = -1;
	usr_mcpwm_cfg.complementary_en = 0;
	usr_mcpwm_cfg.detect_port = -1;
	usr_mcpwm_cfg.irq_cb = NULL;
	usr_mcpwm_cfg.irq_priority = 1;
	g_pwm_pb9_id = mcpwm_init(&usr_mcpwm_cfg);

	usr_mcpwm_cfg.ch = MCPWM_CH0 + 2;
	usr_mcpwm_cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
	usr_mcpwm_cfg.frequency = 1000;	   // frequency Hz
	usr_mcpwm_cfg.duty = 0;			   // duty 0~10000
	usr_mcpwm_cfg.h_pin = IO_PORTB_07; // port
	usr_mcpwm_cfg.l_pin = -1;
	usr_mcpwm_cfg.complementary_en = 0;
	usr_mcpwm_cfg.detect_port = -1;
	usr_mcpwm_cfg.irq_cb = NULL;
	usr_mcpwm_cfg.irq_priority = 1;
	g_pwm_pa1_id = mcpwm_init(&usr_mcpwm_cfg);

	usr_mcpwm_cfg.ch = MCPWM_CH0 + 3;
	usr_mcpwm_cfg.aligned_mode = MCPWM_EDGE_ALIGNED;
	usr_mcpwm_cfg.frequency = 1000;	   // frequency Hz
	usr_mcpwm_cfg.duty = 0;			   // duty 0~10000
	usr_mcpwm_cfg.h_pin = IO_PORTB_08; // port
	usr_mcpwm_cfg.l_pin = -1;
	usr_mcpwm_cfg.complementary_en = 0;
	usr_mcpwm_cfg.detect_port = -1;
	usr_mcpwm_cfg.irq_cb = NULL;
	usr_mcpwm_cfg.irq_priority = 1;
	g_pwm_pa0_id = mcpwm_init(&usr_mcpwm_cfg);

	mcpwm_start(g_pwm_pb10_id);
	mcpwm_start(g_pwm_pb9_id);
	mcpwm_start(g_pwm_pa1_id);
	mcpwm_start(g_pwm_pa0_id);

	return 0;
}



/**
 * @brief Control motor PWM duty cycle with direction support.
 * @param motor_id Motor ID: 0=group1(PB10/PB9), 1=group2(PA1/PA0).
 * @param duty Duty cycle range -10000~10000.
 *             Positive: first pin outputs PWM, second pin outputs 0.
 *             Negative: second pin outputs PWM, first pin outputs 0.
 *             Zero: both pins output 0 (brake).
 */
void motor_set_duty(u8 motor_id, s16 duty)
{
	if (duty > 10000) {
		duty = 10000;
	} else if (duty < -10000) {
		duty = -10000;
	}

	u16 pwm_duty = (duty >= 0) ? duty : (-duty);

	int pin1_id, pin2_id;

	switch (motor_id) {
		case 0:
			pin1_id = g_pwm_pb10_id;
			pin2_id = g_pwm_pb9_id;
			break;
		case 1:
			pin1_id = g_pwm_pa1_id;
			pin2_id = g_pwm_pa0_id;
			break;
		default:
			printf("Error: Invalid motor_id %d\n", motor_id);
			return;
	}

	if (duty > 0) {
		mcpwm_set_duty(pin1_id, pwm_duty);
		mcpwm_set_duty(pin2_id, 0);
	} else if (duty < 0) {
		mcpwm_set_duty(pin1_id, 0);
		mcpwm_set_duty(pin2_id, pwm_duty);
	} else {
		mcpwm_set_duty(pin1_id, 0);
		mcpwm_set_duty(pin2_id, 0);
	}
}

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
	static u32 check_counter = 0;
	
	while (1) {
		// Check music file every 3 seconds
		check_counter++;
		if (check_counter >= 30) {  // 30 * 100ms = 3s
			check_counter = 0;
			
			// Try to read file to check if it exists
			RESFILE *fp;
			char fatfsi_path[32] = "flash/APP/FATFSI/";
			char file_check_path[64] = "flash/APP/FATFSI/music0.mp3";
			int ret = cfg_private_init(5, fatfsi_path);
			if (ret == 0) {
				fp = cfg_private_open_by_maxsize(file_check_path, "r", 16 * 1024);
				if (fp) {
					// Get file size using resfile_get_attrs
					struct resfile_attrs attrs;
					if (resfile_get_attrs(fp, &attrs) == 0) {
						int file_size = attrs.fsize;
						
						if (file_size > 0) {
							// Read first 5 bytes
							u8 first_5[5] = {0};
							u8 last_5[5] = {0};
							
							cfg_private_seek(fp, 0, SEEK_SET);
							cfg_private_read(fp, first_5, 5);
							
							// Read last 5 bytes
							if (file_size >= 5) {
								cfg_private_seek(fp, file_size - 5, SEEK_SET);
								cfg_private_read(fp, last_5, 5);
							}
							
							printf("[CHECK] music0.mp3 exists, size=%d bytes\n", file_size);
							printf("[CHECK] First 5: %02X %02X %02X %02X %02X\n", 
								   first_5[0], first_5[1], first_5[2], first_5[3], first_5[4]);
							printf("[CHECK] Last 5:  %02X %02X %02X %02X %02X\n", 
								   last_5[0], last_5[1], last_5[2], last_5[3], last_5[4]);
						}
					}
					cfg_private_close(fp);
				} else {
					printf("[CHECK] music0.mp3 not found\n");
				}
				cfg_private_uninit();
			}
		}
		
		os_time_dly(100);  // Check every 100ms
	}
}


/**
 * @brief BLE file transfer state machine.
 */
static struct {
	u8 receiving;
	u8 *file_buf;
	u32 file_size;
	u32 received_bytes;
} ble_file_transfer = {0};

#define BLE_FILE_BUF_SIZE (16 * 1024)

/**
 * @brief Handles BLE notification or indication data received from a remote device
 * Protocol: 0xAA 0x55 CMD LEN_L LEN_H [DATA...]
 * CMD: 0x01=Start, 0x02=Data, 0x03=End
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
	printf("[BLE RX] hdl:%d, len:%d\n", ble_con_hdl, len);
	
	if (len < 5) return;
	
	if (buf[0] != 0xAA || buf[1] != 0x55) {
		printf("[BLE] Invalid header\n");
		return;
	}
	
	u8 cmd = buf[2];
	u16 data_len = buf[3] | (buf[4] << 8);
	u8 *data = &buf[5];
	
	switch (cmd) {
		case 0x01: // Start
			printf("[BLE] Start receiving file, size=%d\n", data_len);
			if (ble_file_transfer.file_buf) {
				free(ble_file_transfer.file_buf);
			}
			ble_file_transfer.file_buf = (u8 *)malloc(BLE_FILE_BUF_SIZE);
			if (!ble_file_transfer.file_buf) {
				printf("[BLE] Malloc failed\n");
				return;
			}
			ble_file_transfer.file_size = data_len;
			ble_file_transfer.received_bytes = 0;
			ble_file_transfer.receiving = 1;
			printf("[BLE] Ready to receive\n");
			break;
			
		case 0x02: // Data
			if (!ble_file_transfer.receiving) {
				printf("[BLE] Not in receiving state\n");
				return;
			}
			if (ble_file_transfer.received_bytes + data_len > BLE_FILE_BUF_SIZE) {
				printf("[BLE] Buffer overflow\n");
				return;
			}
			memcpy(ble_file_transfer.file_buf + ble_file_transfer.received_bytes, data, data_len);
			ble_file_transfer.received_bytes += data_len;
			printf("[BLE] Received %d/%d bytes\n", ble_file_transfer.received_bytes, ble_file_transfer.file_size);
			break;
			
		case 0x03: // End
			if (!ble_file_transfer.receiving) {
				printf("[BLE] Not in receiving state\n");
				return;
			}
			printf("[BLE] Transfer complete, saving to flash...\n");
			int ret = user_music_file_rw(ble_file_transfer.file_buf, 
										 ble_file_transfer.received_bytes, 
										 NULL, 0);
			if (ret == 0) {
				printf("[BLE] File saved successfully\n");
			} else {
				printf("[BLE] File save failed: %d\n", ret);
			}
			
			ble_file_transfer.receiving = 0;
			if (ble_file_transfer.file_buf) {
				free(ble_file_transfer.file_buf);
				ble_file_transfer.file_buf = NULL;
			}
			ble_file_transfer.received_bytes = 0;
			ble_file_transfer.file_size = 0;
			break;
			
		default:
			printf("[BLE] Unknown command: 0x%02X\n", cmd);
			break;
	}
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

		fp = cfg_private_open_by_maxsize(file_path, "w+", 32 * 1024);
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
		
		fp = cfg_private_open_by_maxsize(file_path, "r", 32 * 1024);
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
	pwm_init();
	os_task_create(user_blink_task,
				   NULL,
				   2,
				   256,
				   0,
				   "user_blink");
}
