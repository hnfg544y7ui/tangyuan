#ifndef __KEY_CHECK_H__
#define __KEY_CHECK_H__

#include "system/includes.h"

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT_PRESS,
    KEY_EVENT_LONG_PRESS,
    KEY_EVENT_DOUBLE_CLICK,
    KEY_EVENT_TRIPLE_CLICK,
} key_event_t;

typedef void (*key_event_callback_t)(key_event_t t_event);

/**
 * @brief Initialize touch key detection module.
 * @param t_callback Callback function for key events.
 * @return 0 if success, negative value on error.
 */
int key_check_init(key_event_callback_t t_callback);

#endif
