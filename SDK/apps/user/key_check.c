#include "key_check.h"
#include "gpio.h"

#define KEY_CHECK_DEBUG_ENABLE  1
#if KEY_CHECK_DEBUG_ENABLE
#define key_check_debug(fmt, ...) printf("[KEY_CHECK] "fmt, ##__VA_ARGS__)
#else
#define key_check_debug(...)
#endif

#define KEY_NUM             2
#define KEY_0_PIN           IO_PORTB_00
#define KEY_1_PIN           IO_PORTB_01
#define LONG_PRESS_TIME     200   // 2s (200*10ms)
#define CLICK_INTERVAL      50    // 500ms (50*10ms)

typedef enum {
    KEY_POLARITY_LOW = 0,   //Active low: pressed=0, released=1
    KEY_POLARITY_HIGH = 1,  //Active high: pressed=1, released=0
} key_polarity_t;

typedef enum {
    KEY_STATE_IDLE = 0,
    KEY_STATE_PRESSED,
    KEY_STATE_WAIT_RELEASE,
    KEY_STATE_WAIT_CLICK,
} key_state_t;

static struct {
    u32 pin;
    key_polarity_t polarity;
    key_state_t state;
    u16 press_count;
    u16 click_count;
    u16 idle_count;
    key_event_callback_t callback;
} g_key_ctx[KEY_NUM] = {0};

/**
 * @brief Touch key detection task.
 * @param t_param Task parameter.
 */
static void touch_key_task(void *t_param)
{
    u8 key_level;
    u8 is_pressed;
    
    while (1) {
        for (u8 i = 0; i < KEY_NUM; i++) {
            key_level = gpio_read(g_key_ctx[i].pin);
            
            /* Determine if key is pressed based on polarity */
            if (g_key_ctx[i].polarity == KEY_POLARITY_LOW) {
                is_pressed = (key_level == 0);
            } else {
                is_pressed = (key_level == 1);
            }
            
            switch (g_key_ctx[i].state) {
                case KEY_STATE_IDLE:{
                    if (is_pressed) {
                        g_key_ctx[i].state = KEY_STATE_PRESSED;
                        g_key_ctx[i].press_count = 0;
                    }
                }break;
                
                case KEY_STATE_PRESSED:{
                    if (is_pressed) {
                        g_key_ctx[i].press_count++;
                        if (g_key_ctx[i].press_count >= LONG_PRESS_TIME) {
                            if (g_key_ctx[i].callback) {
                                g_key_ctx[i].callback((key_id_t)i, KEY_EVENT_LONG_PRESS);
                            }
                            g_key_ctx[i].state = KEY_STATE_WAIT_RELEASE;
                            g_key_ctx[i].press_count = 0;
                        }
                    } else {
                        /* Short press detected */
                        g_key_ctx[i].click_count++;
                        g_key_ctx[i].state = KEY_STATE_WAIT_CLICK;
                        g_key_ctx[i].idle_count = 0;
                    }
                }break;
                
                case KEY_STATE_WAIT_RELEASE:{
                    if (!is_pressed) {
                        g_key_ctx[i].state = KEY_STATE_IDLE;
                    }
                }break;
                
                case KEY_STATE_WAIT_CLICK:{
                    g_key_ctx[i].idle_count++;
                    
                    if (is_pressed) {
                        /* Another click detected */
                        g_key_ctx[i].state = KEY_STATE_PRESSED;
                        g_key_ctx[i].press_count = 0;
                        g_key_ctx[i].idle_count = 0;
                    } else if (g_key_ctx[i].idle_count >= CLICK_INTERVAL) {
                        /* Click sequence completed */
                        if (g_key_ctx[i].callback) {
                            switch (g_key_ctx[i].click_count) {
                                case 1:{
                                    g_key_ctx[i].callback((key_id_t)i, KEY_EVENT_SHORT_PRESS);
                                }break;
                                case 2:{
                                    g_key_ctx[i].callback((key_id_t)i, KEY_EVENT_DOUBLE_CLICK);
                                }break;
                                case 3:{
                                    g_key_ctx[i].callback((key_id_t)i, KEY_EVENT_TRIPLE_CLICK);
                                }break;
                                default:
                                    break;
                            }
                        }
                        g_key_ctx[i].state = KEY_STATE_IDLE;
                        g_key_ctx[i].click_count = 0;
                        g_key_ctx[i].idle_count = 0;
                    }
                }break;
            }
        }
        
        os_time_dly(1);
    }
}

/**
 * @brief Initialize touch key detection module.
 * @param t_callback Callback function for key events.
 * @return 0 if success, negative value on error.
 */
int key_check_init(key_event_callback_t t_callback)
{
    gpio_set_mode(PORTB, PORT_PIN_0, PORT_INPUT_PULLUP_10K);
    gpio_set_mode(PORTB, PORT_PIN_1, PORT_INPUT_PULLUP_10K);
    
    /* KEY0: Touch key, active high (pressed=1, released=0) */
    g_key_ctx[0].pin = KEY_0_PIN;
    g_key_ctx[0].polarity = KEY_POLARITY_HIGH;
    g_key_ctx[0].state = KEY_STATE_IDLE;
    g_key_ctx[0].press_count = 0;
    g_key_ctx[0].click_count = 0;
    g_key_ctx[0].idle_count = 0;
    g_key_ctx[0].callback = t_callback;
    
    /* KEY1: Mechanical key, active low (pressed=0, released=1) */
    g_key_ctx[1].pin = KEY_1_PIN;
    g_key_ctx[1].polarity = KEY_POLARITY_LOW;
    g_key_ctx[1].state = KEY_STATE_IDLE;
    g_key_ctx[1].press_count = 0;
    g_key_ctx[1].click_count = 0;
    g_key_ctx[1].idle_count = 0;
    g_key_ctx[1].callback = t_callback;
    
    int ret = os_task_create(touch_key_task,
                             NULL,
                             5,
                             256,
                             0,
                             "key_check");
    
    if (ret == 0) {
        key_check_debug("Key module initialized\n");
    }
    
    return ret;
}
