#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PROFILING_MAX_STOPWATCHES 8

// Ограничение: сумма замеров хранится в 32 битах.
// Для одного секундомера переполнение наступит примерно через 71 минуту.
typedef struct {
    const char *name;
    uint32_t count;
    uint32_t mean_us;
    uint32_t max_us;
} profiling_result_t;

void profiling_init(void);
void profiling_stopwatch_init(uint32_t id, const char *name);
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);