#pragma once
#include <stdbool.h>

// Запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

// Управление светодиодом
void platform_led_set(bool on);

// Нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void);