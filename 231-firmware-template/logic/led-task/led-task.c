#include "led-task/led-task.h" // Свой заголовок первым!
#include "platform/platform.h" // <-- Используем платформу
#include "systime/systime.h"

static led_state_t current_state = LED_STATE_BLINK;
static uint32_t current_period_ms = 1000;
static uint64_t last_toggle_us = 0;
static bool led_is_on = false; // Запоминаем состояние для переключения

void led_task_init(void) {
    current_period_ms = 1000;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state) {
    current_state = state;
    
    switch (state) {
        case LED_STATE_OFF:
            led_is_on = false;
            platform_led_set(led_is_on);
            break;
        case LED_STATE_ON:
            led_is_on = true;
            platform_led_set(led_is_on);
            break;
        case LED_STATE_BLINK:
            last_toggle_us = systime_us() - ((uint64_t)current_period_ms * 1000 / 2);
            break;
    }
}

led_state_t led_task_get_state(void) { return current_state; }

bool led_task_set_period_ms(uint32_t period_ms) {
    if (period_ms == 0) return false;
    current_period_ms = period_ms;
    return true;
}

uint32_t led_task_get_period_ms(void) { return current_period_ms; }

void led_task_handle(void) {
    switch (current_state) {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            break;
        case LED_STATE_BLINK:
            if (systime_period_elapsed(&last_toggle_us, (uint64_t)current_period_ms * 1000 / 2)) {
                led_is_on = !led_is_on;
                platform_led_set(led_is_on); // <-- Используем платформу
            }
            break;
    }
}

void led_task_next_state(void) {
    switch (current_state) {
        case LED_STATE_OFF:   led_task_set_state(LED_STATE_ON); break;
        case LED_STATE_ON:    led_task_set_state(LED_STATE_BLINK); break;
        case LED_STATE_BLINK: led_task_set_state(LED_STATE_OFF); break;
    }
}