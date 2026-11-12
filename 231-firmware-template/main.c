#include <stdio.h>
#include "pico/stdlib.h"

#include "device.h"
#include "profiling.h"
#include "api.h"
#include "stdio-text-protocol.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"
#include "log.h" // <-- ДОБАВЛЕНО для LOG_INF

// Номера секундомеров
typedef enum {
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI,
    STOPWATCH_COUNT
} stopwatch_id_t;

int main() {
    stdio_init_all();
    
    // Вывод версии при запуске
    LOG_INF("%s %s\n", DEVICE_NAME, FIRMWARE_VERSION);
    
    sleep_ms(2000);
    
    profiling_init();
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    stdio_text_protocol_init();
    led_task_init(); 
    button_task_init(); 
    pi_task_init(); 

    while (1) {
        profiling_start(STOPWATCH_LOOP);

        // Светодиод
        profiling_start(STOPWATCH_LED);
        led_task_handle(); 
        profiling_stop(STOPWATCH_LED);

        // Кнопка
        profiling_start(STOPWATCH_BUTTON);
        button_task_handle(); 
        profiling_stop(STOPWATCH_BUTTON);

        // Пи
        profiling_start(STOPWATCH_PI);
        pi_task_handle(); 
        profiling_stop(STOPWATCH_PI);

        // Прием и обработка команды
        profiling_start(STOPWATCH_COMMAND);
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL) {
            api_handle(command);
        }
        profiling_stop(STOPWATCH_COMMAND);

        profiling_stop(STOPWATCH_LOOP);
    }
    return 0;
}