#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PI_STATE_NOT_STARTED,
    PI_STATE_RUNNING,
    PI_STATE_DONE
} pi_state_t;

void pi_task_init(void);

// Запуск расчета. Возвращает false, если расчет уже идет.
bool pi_task_start(uint32_t terms);

// Вызывается в суперцикле, считает порцию
void pi_task_handle(void);

// Получение результатов
pi_state_t pi_task_get_state(void);
double pi_task_get_result(void);
uint64_t pi_task_get_time_ms(void);