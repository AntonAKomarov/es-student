#include "platform/platform.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25
static const uint BUTTON_PINS[BUTTON_COUNT]={15, 14};

void platform_init(void) {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    for (int i=0; i<BUTTON_COUNT; i++){
    gpio_init(BUTTON_PINS[i]);
    gpio_set_dir(BUTTON_PINS[i], GPIO_IN);
    gpio_pull_up(BUTTON_PINS[i]); // Кнопка замыкает на землю

    }



}

void platform_led_set(bool on) {
    gpio_put(LED_PIN, on);
}

bool platform_button_read(platform_button_t button) {
    if (button>=BUTTON_COUNT) return false;
    
    // Нажатая кнопка замыкает на землю и дает 0 (false)
    return !gpio_get(BUTTON_PINS[button]);
}