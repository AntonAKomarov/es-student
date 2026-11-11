#include "pi-task.h"
#include "pico/stdlib.h"

// Порция: около 100 членов ряда, что занимает ~0.5 мс
#define PI_TERMS_PER_CALL 100

// Переменные файла (внутреннее связывание)
static pi_state_t state = PI_STATE_NOT_STARTED;
static double sum = 0.0;
static uint32_t current_term = 0;
static uint32_t total_terms = 0;
static uint64_t start_time_us = 0;
static uint64_t end_time_us = 0;

void pi_task_init(void) {
    state = PI_STATE_NOT_STARTED;
}

bool pi_task_start(uint32_t terms) {
    if (state == PI_STATE_RUNNING) {
        return false;
    }
    
    sum = 0.0;
    current_term = 0;
    total_terms = terms;
    start_time_us = time_us_64();
    state = PI_STATE_RUNNING;
    return true;
}

void pi_task_handle(void) {
    if (state != PI_STATE_RUNNING) {
        return;
    }

    // Считаем конец порции с защитой от переполнения uint32_t
    uint32_t end = current_term + PI_TERMS_PER_CALL;
    if (end < current_term || end > total_terms) {
        end = total_terms;
    }

    // Считаем порцию
    for (uint32_t i = current_term; i < end; i++) {
        double k = (i % 2 == 0) ? 1.0 : -1.0;
        sum += k / (2.0 * i + 1.0);
    }

    current_term = end;

    // Если достигли конца — завершаем
    if (current_term >= total_terms) {
        end_time_us = time_us_64();
        state = PI_STATE_DONE;
    }
}

pi_state_t pi_task_get_state(void) {
    return state;
}

double pi_task_get_result(void) {
    return sum * 4.0;
}

uint64_t pi_task_get_time_ms(void) {
    if (state == PI_STATE_DONE) {
        return (end_time_us - start_time_us) / 1000;
    }
    return 0;
}