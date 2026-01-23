#include "key_check.h"
#include "gpio.h"

#define TOUCH_KEY_PIN       IO_PORTB_00
#define LONG_PRESS_TIME     200   // 2s (200*10ms)
#define CLICK_INTERVAL      50    // 500ms (50*10ms)

typedef enum {
    KEY_STATE_IDLE = 0,
    KEY_STATE_PRESSED,
    KEY_STATE_WAIT_RELEASE,
    KEY_STATE_WAIT_CLICK,
} key_state_t;

static struct {
    key_state_t state;
    u16 press_count;
    u16 click_count;
    u16 idle_count;
    key_event_callback_t callback;
} g_key_ctx = {0};

/**
 * @brief Touch key detection task.
 * @param t_param Task parameter.
 */
static void touch_key_task(void *t_param)
{
    u8 key_level;
    
    while (1) {
        key_level = gpio_read(TOUCH_KEY_PIN);
        
        switch (g_key_ctx.state) {
            case KEY_STATE_IDLE:{
                if (key_level == 0) {
                    g_key_ctx.state = KEY_STATE_PRESSED;
                    g_key_ctx.press_count = 0;
                }
            }break;
            
            case KEY_STATE_PRESSED:{
                if (key_level == 0) {
                    g_key_ctx.press_count++;
                    if (g_key_ctx.press_count >= LONG_PRESS_TIME) {
                        if (g_key_ctx.callback) {
                            g_key_ctx.callback(KEY_EVENT_LONG_PRESS);
                        }
                        g_key_ctx.state = KEY_STATE_WAIT_RELEASE;
                        g_key_ctx.press_count = 0;
                    }
                } else {
                    /* Short press detected */
                    g_key_ctx.click_count++;
                    g_key_ctx.state = KEY_STATE_WAIT_CLICK;
                    g_key_ctx.idle_count = 0;
                }
            }break;
            
            case KEY_STATE_WAIT_RELEASE:{
                if (key_level == 1) {
                    g_key_ctx.state = KEY_STATE_IDLE;
                }
            }break;
            
            case KEY_STATE_WAIT_CLICK:{
                g_key_ctx.idle_count++;
                
                if (key_level == 0) {
                    /* Another click detected */
                    g_key_ctx.state = KEY_STATE_PRESSED;
                    g_key_ctx.press_count = 0;
                    g_key_ctx.idle_count = 0;
                } else if (g_key_ctx.idle_count >= CLICK_INTERVAL) {
                    /* Click sequence completed */
                    if (g_key_ctx.callback) {
                        switch (g_key_ctx.click_count) {
                            case 1:{
                                g_key_ctx.callback(KEY_EVENT_SHORT_PRESS);
                            }break;
                            case 2:{
                                g_key_ctx.callback(KEY_EVENT_DOUBLE_CLICK);
                            }break;
                            case 3:{
                                g_key_ctx.callback(KEY_EVENT_TRIPLE_CLICK);
                            }break;
                            default:
                                break;
                        }
                    }
                    g_key_ctx.state = KEY_STATE_IDLE;
                    g_key_ctx.click_count = 0;
                    g_key_ctx.idle_count = 0;
                }
            }break;
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
    
    g_key_ctx.state = KEY_STATE_IDLE;
    g_key_ctx.press_count = 0;
    g_key_ctx.click_count = 0;
    g_key_ctx.idle_count = 0;
    g_key_ctx.callback = t_callback;
    
    int ret = os_task_create(touch_key_task,
                             NULL,
                             5,
                             256,
                             0,
                             "key_check");
    
    if (ret == 0) {
        printf("Touch key initialized (PB0)\n");
    }
    
    return ret;
}
