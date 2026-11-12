#include "profiling/profiling.h"
#include "pico/stdlib.h"
#include <stddef.h>

typedef struct {
    const char *name;
    uint32_t start_us;
    uint32_t sum_us;
    uint32_t count;
    uint32_t max_us;
    bool running;
} stopwatch_t;

static stopwatch_t stopwatches[PROFILING_MAX_STOPWATCHES];

void profiling_init(void) {
    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) {
        stopwatches[i].name = NULL;
        stopwatches[i].sum_us = 0;
        stopwatches[i].count = 0;
        stopwatches[i].max_us = 0;
        stopwatches[i].running = false;
    }
}

void profiling_stopwatch_init(uint32_t id, const char *name) {
    if (id >= PROFILING_MAX_STOPWATCHES) return;
    stopwatches[id].name = name;
    stopwatches[id].sum_us = 0;
    stopwatches[id].count = 0;
    stopwatches[id].max_us = 0;
    stopwatches[id].running = false;
}

#if PROFILING_ENABLED
void profiling_start(uint32_t id) {
    if (id >= PROFILING_MAX_STOPWATCHES) return;
    stopwatches[id].start_us = time_us_32();
    stopwatches[id].running = true;
}

void profiling_stop(uint32_t id) {
    if (id >= PROFILING_MAX_STOPWATCHES) return;
    if (!stopwatches[id].running) return;
    
    uint32_t now_us = time_us_32();
    uint32_t duration = now_us - stopwatches[id].start_us;
    
    stopwatches[id].sum_us += duration;
    stopwatches[id].count++;
    if (duration > stopwatches[id].max_us) {
        stopwatches[id].max_us = duration;
    }
    stopwatches[id].running = false;
}
#endif

bool profiling_get(uint32_t id, profiling_result_t *result) {
    if (id >= PROFILING_MAX_STOPWATCHES) return false;
    if (stopwatches[id].name == NULL) return false;
    
    result->name = stopwatches[id].name;
    result->count = stopwatches[id].count;
    result->max_us = stopwatches[id].max_us;
    
    // Среднее арифметическое
    if (stopwatches[id].count > 0) {
        result->mean_us = stopwatches[id].sum_us / stopwatches[id].count;
    } else {
        result->mean_us = 0;
    }
    return true;
}

void profiling_reset(void) {
    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) {
        stopwatches[i].sum_us = 0;
        stopwatches[i].count = 0;
        stopwatches[i].max_us = 0;
    }
}