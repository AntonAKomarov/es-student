#include "button-task/button-task.h" 
#include <stddef.h>
#include "platform/platform.h"       
#include "systime/systime.h"

#define DEBOUNCE_TIME_US 20000

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESS_WAIT,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASE_WAIT
} button_state_t;

static button_state_t current_state;
static uint32_t press_count;
static uint64_t last_change_us;
static button_task_callback_t press_callback; 

void button_task_init(button_task_callback_t on_press) {
    current_state = BUTTON_STATE_RELEASED;
    press_count = 0;
    last_change_us = 0;
    press_callback = on_press;
}

void button_task_handle(void) {
    bool is_pressed = platform_button_read(); 
    uint64_t now_us = systime_us();

    switch (current_state) {
        case BUTTON_STATE_RELEASED:
            if (is_pressed) {
                last_change_us = now_us;
                current_state = BUTTON_STATE_PRESS_WAIT;
            }
            break;
        case BUTTON_STATE_PRESS_WAIT:
            if (!is_pressed) {
                current_state = BUTTON_STATE_RELEASED;
            } else if (now_us - last_change_us >= DEBOUNCE_TIME_US) {
                current_state = BUTTON_STATE_PRESSED;
                press_count++;
                if (press_callback != NULL) { 
                    press_callback();
                }
            }
            break;
        case BUTTON_STATE_PRESSED:
            if (!is_pressed) {
                last_change_us = now_us;
                current_state = BUTTON_STATE_RELEASE_WAIT;
            }
            break;
        case BUTTON_STATE_RELEASE_WAIT:
            if (is_pressed) {
                current_state = BUTTON_STATE_PRESSED;
            } else if (now_us - last_change_us >= DEBOUNCE_TIME_US) {
                current_state = BUTTON_STATE_RELEASED;
            }
            break;
    }
}

bool button_task_is_pressed(void) {
    return (current_state == BUTTON_STATE_PRESSED || current_state == BUTTON_STATE_RELEASE_WAIT);
}

uint32_t button_task_get_press_count(void) {
    return press_count;
}