#include <stdio.h>
#include "pico/stdlib.h"

#include "device.h"
#include "profiling.h"
#include "api.h"
#include "stdio-text-protocol.h"
#include "led-task.h"
#include "button-task.h" // <-- ДОБАВЛЕНО

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    profiling_init();
    stdio_text_protocol_init();
    led_task_init(); 
    button_task_init(); // <-- ДОБАВЛЕНО

    while (1) {
        profiling_iteration();
        
        led_task_handle(); 
        button_task_handle(); // <-- ДОБАВЛЕНО (опрос кнопки)

        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL) {
            api_handle(command);
        }
    }
    return 0;
}