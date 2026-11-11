#pragma once

#include <stdbool.h>
#include <stdint.h>

void button_task_init(void);
void button_task_handle(void);

// Возвращает true, если кнопка нажата (включая состояние подтверждения нажатия)
bool button_task_is_pressed(void);

// Возвращает количество зафиксированных нажатий
uint32_t button_task_get_press_count(void);