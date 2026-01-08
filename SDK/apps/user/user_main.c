#include "system/includes.h"
#include "gpio.h"
#include "user_main.h"
#include "btstack/third_party/rcsp/btstack_rcsp_user.h"
#include "btstack/le/att.h"
#include "asm/mcpwm.h"

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
