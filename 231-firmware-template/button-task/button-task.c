#include "button-task.h"
#include "led-task.h" // Кнопка вызывает функцию светодиода
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_PIN 15
#define DEBOUNCE_TIME_US 20000 // 20 мс

// Машина состояний кнопки
typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESS_WAIT,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASE_WAIT
} button_state_t;

static button_state_t current_state;
static uint32_t press_count;
static uint64_t last_change_us;

void button_task_init(void) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Подтяжка к питанию (кнопка замыкает на землю)

    current_state = BUTTON_STATE_RELEASED;
    press_count = 0;
    last_change_us = 0;
}

void button_task_handle(void) {
    // Читаем уровень: нажатая кнопка замыкает вывод на землю и даёт 0 (false)
    bool is_low = !gpio_get(BUTTON_PIN); 
    uint64_t now_us = time_us_64();

    switch (current_state) {
        case BUTTON_STATE_RELEASED:
            if (is_low) {
                last_change_us = now_us;
                current_state = BUTTON_STATE_PRESS_WAIT;
            }
            break;

        case BUTTON_STATE_PRESS_WAIT:
            if (!is_low) { // Если отпустили (дребезг)
                current_state = BUTTON_STATE_RELEASED;
            } else if (now_us - last_change_us >= DEBOUNCE_TIME_US) {
                // Продержали 20 мс — фиксируем нажатие
                current_state = BUTTON_STATE_PRESSED;
                press_count++;
                led_task_next_state(); // Переключаем светодиод
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (!is_low) { // Кнопку начали отпускать
                last_change_us = now_us;
                current_state = BUTTON_STATE_RELEASE_WAIT;
            }
            break;

        case BUTTON_STATE_RELEASE_WAIT:
            if (is_low) { // Если снова нажали (дребезг)
                current_state = BUTTON_STATE_PRESSED;
            } else if (now_us - last_change_us >= DEBOUNCE_TIME_US) {
                // Продержали отпущенной 20 мс — фиксируем отпускание
                current_state = BUTTON_STATE_RELEASED;
            }
            break;
    }
}

bool button_task_is_pressed(void) {
    // Кнопка считается нажатой, пока не подтвердилось отпускание
    return (current_state == BUTTON_STATE_PRESSED || current_state == BUTTON_STATE_RELEASE_WAIT);
}

uint32_t button_task_get_press_count(void) {
    return press_count;
}