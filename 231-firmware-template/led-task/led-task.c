#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25


static led_state_t current_state = LED_STATE_BLINK;
static uint32_t current_period_ms = 1000;
static uint64_t last_toggle_us = 0;


static void led_set(bool on) {
    gpio_put(LED_PIN, on);
}

// Проверка периода: отсчет от прошлого срабатывания
static bool period_elapsed(uint64_t *last_us, uint64_t period_us) {
    uint64_t now_us = time_us_64();
    if (now_us - *last_us >= period_us) {
        *last_us += period_us;
        if (now_us - *last_us >= period_us) {
            *last_us = now_us;
        }
        return true;
    }
    return false;
}

void led_task_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    current_period_ms = 1000;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state) {
    current_state = state;
    
    // Действие при входе в состояние
    switch (state) {
        case LED_STATE_OFF:
            led_set(false);
            break;
        case LED_STATE_ON:
            led_set(true);
            break;
        case LED_STATE_BLINK:
            // Метка прошлого переключения указывает в прошлое на полпериода, 
            // чтобы первое переключение случилось сразу
            last_toggle_us = time_us_64() - ((uint64_t)current_period_ms * 1000 / 2);
            break;
    }
}

led_state_t led_task_get_state(void) {
    return current_state;
}

bool led_task_set_period_ms(uint32_t period_ms) {
    if (period_ms == 0) {
        return false; // Нулевой период не принимаем
    }
    current_period_ms = period_ms;
    return true;
}

uint32_t led_task_get_period_ms(void) {
    return current_period_ms;
}

void led_task_handle(void) {
    switch (current_state) {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            // Светодиод уже установлен при входе, ничего не делаем
            break;
        case LED_STATE_BLINK:
            // Каждые полпериода переключаем светодиод
            if (period_elapsed(&last_toggle_us, (uint64_t)current_period_ms * 1000 / 2)) {
                led_set(!gpio_get(LED_PIN));
            }
            break;
    }
}

void led_task_next_state(void) {
    switch (current_state) {
        case LED_STATE_OFF:
            led_task_set_state(LED_STATE_ON);
            break;
        case LED_STATE_ON:
            led_task_set_state(LED_STATE_BLINK);
            break;
        case LED_STATE_BLINK:
            led_task_set_state(LED_STATE_OFF);
            break;
    }
}