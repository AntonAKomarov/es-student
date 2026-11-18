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

typedef struct {
    button_state_t state;
    uint32_t press_count;
    uint64_t last_change_us;
    button_task_callback_t callback;
} button_data_t;

static button_data_t button_data[BUTTON_COUNT];

void button_task_init(platform_button_t btn, button_task_callback_t on_press) {
    if (btn >= BUTTON_COUNT) return;

    button_data[btn].state = BUTTON_STATE_RELEASED;
    button_data[btn].press_count = 0;
    button_data[btn].last_change_us = 0;
    button_data[btn].callback = on_press;
}

void button_task_handle(void) {
    uint64_t now_us = systime_us();
    for (int i = 0; i < BUTTON_COUNT; i++) {
        bool is_pressed = platform_button_read((platform_button_t)i);

        switch (button_data[i].state) {
            case BUTTON_STATE_RELEASED:
                if (is_pressed) {
                    button_data[i].last_change_us = now_us;
                    button_data[i].state = BUTTON_STATE_PRESS_WAIT;
                }
                break;
            case BUTTON_STATE_PRESS_WAIT:
                if (!is_pressed) {
                    button_data[i].state = BUTTON_STATE_RELEASED;
                } else if (now_us - button_data[i].last_change_us >= DEBOUNCE_TIME_US) {
                    button_data[i].state = BUTTON_STATE_PRESSED;
                    button_data[i].press_count++;
                    if (button_data[i].callback != NULL) {
                        button_data[i].callback();
                    }
                }
                break;
            case BUTTON_STATE_PRESSED:
                if (!is_pressed) {
                    button_data[i].last_change_us = now_us;
                    button_data[i].state = BUTTON_STATE_RELEASE_WAIT;
                }
                break;
            case BUTTON_STATE_RELEASE_WAIT:
                if (is_pressed) {
                    button_data[i].state = BUTTON_STATE_PRESSED;
                } else if (now_us - button_data[i].last_change_us >= DEBOUNCE_TIME_US) {
                    button_data[i].state = BUTTON_STATE_RELEASED;
                }
                break;
        }
    }
}

bool button_task_is_pressed(platform_button_t button) {
    if (button >= BUTTON_COUNT) return false;
    return (button_data[button].state == BUTTON_STATE_PRESSED ||
            button_data[button].state == BUTTON_STATE_RELEASE_WAIT);
}

uint32_t button_task_get_press_count(platform_button_t button) {
    if (button >= BUTTON_COUNT) return 0;
    return button_data[button].press_count;
}