#pragma once
#include <stdbool.h>
typedef enum { BUTTON_1, BUTTON_2, BUTTON_COUNT } platform_button_t;
// Запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

// Управление светодиодом
void platform_led_set(bool on);

// Нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(platform_button_t button);