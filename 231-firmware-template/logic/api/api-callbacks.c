#include "api-commands.h"
#include "device/device.h"
#include "logging/log.h"        
#include "profiling/profiling.h"
#include "systime/systime.h"    
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "firmware.h"           
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

// --- Строгий парсер из т2.3.8 ---
static bool parse_u32(const char *text, uint32_t *value) {
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0])) {
        return false;
    }
    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);
    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX) {
        return false;
    }
    *value = (uint32_t)number;
    return true;
}

// --- Старые команды ---
static void cmd_info(const command_t *command) {
    (void)command;
    device_info_t info;
    device_get_info(&info);

    // Используем FIRMWARE_*
    printf("name: %s\n", FIRMWARE_NAME);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    
    printf("board: %s\n", info.board);
    printf("serial: %s\n", info.serial);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", (unsigned)info.manufacturer, (unsigned)info.part, (unsigned)info.revision);
    printf("pico-sdk: %s\n", info.sdk_version);
}

static void cmd_uptime(const command_t *command) {
    (void)command;
    printf("uptime: %llu ms\n", (unsigned long long)systime_ms()); // <-- systime
}

static void cmd_profiling(const command_t *command) {
    (void)command;
#if PROFILING_ENABLED
    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) {
        profiling_result_t res;
        if (profiling_get(i, &res)) {
            printf("%s count %u mean %.2f us max %u us\n", 
                   res.name, (unsigned)res.count, (unsigned)res.mean_us, (unsigned)res.max_us);
        }
    }
#else
    printf("profiling is disabled in this build\n");
#endif
}

static void cmd_profiling_reset(const command_t *command) {
    (void)command;
    profiling_reset();
    printf("profiling reset\n");
}

static void cmd_pi_start(const command_t *command) {
    uint32_t terms = 1000000; // По умолчанию
    
    if (command->argc > 1) {
        printf("error: usage pi_start [<terms>]\n");
        return;
    }
    if (command->argc == 1) {
        // Сначала проверяем, что это валидное число
        if (!parse_u32(command->argv[0], &terms)) {
            printf("error: usage pi_start [<terms>]\n");
            return;
        }
        // Затем проверяем на ноль
        if (terms == 0) {
            printf("error: terms must be greater than 0\n");
            return;
        }
    }
    
    // Пытаемся запустить расчет
    if (!pi_task_start(terms)) {
        printf("error: pi is busy\n");
        return;
    }
    
    printf("pi: started, %u terms\n", (unsigned)terms);
}

static void cmd_pi(const command_t *command) {
    (void)command;
    switch (pi_task_get_state()) {
        case PI_STATE_NOT_STARTED:
            printf("pi: not started\n");
            break;
        case PI_STATE_RUNNING:
            printf("pi: running\n");
            break;
        case PI_STATE_DONE:
            printf("pi: %.9f in %llu ms\n", pi_task_get_result(), (unsigned long long)pi_task_get_time_ms());
            break;
    }
}

// --- Новые команды светодиода ---
static void print_led_status(void) {
    const char* state_str = "";
    switch (led_task_get_state()) {
        case LED_STATE_OFF:   state_str = "off"; break;
        case LED_STATE_ON:    state_str = "on"; break;
        case LED_STATE_BLINK: state_str = "blink"; break;
    }
    printf("led: %s, period %u ms\n", state_str, (unsigned)led_task_get_period_ms());
}

static void cmd_led_enable(const command_t *command) {
    (void)command;
    led_task_set_state(LED_STATE_ON);
    print_led_status();
}

static void cmd_led_disable(const command_t *command) {
    (void)command;
    led_task_set_state(LED_STATE_OFF);
    print_led_status();
}

static void cmd_led_blink(const command_t *command) {
    (void)command;
    led_task_set_state(LED_STATE_BLINK);
    print_led_status();
}

static void cmd_led_period(const command_t *command) {
    uint32_t period_ms;
    
    // Проверка: ровно 1 аргумент и он является валидным числом
    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms)) {
        printf("error: usage led_period <period_ms>\n");
        return;
    }
    
    // Годится ли такой период (не ноль), решает задача светодиода
    if (!led_task_set_period_ms(period_ms)) {
        printf("error: period_ms must be greater than 0\n");
        return;
    }
    print_led_status();
}

static void cmd_button(const command_t *command) {
    (void)command;
    const char* state_str = button_task_is_pressed() ? "pressed" : "released";
    printf("button: %s, presses %u\n", state_str, (unsigned)button_task_get_press_count());
}

// Таблица команд
const api_command_t api_commands[] = {
    {"info", "device passport", cmd_info},
    {"uptime", "time since reset", cmd_uptime},
    {"profiling", "show all stopwatches", cmd_profiling},             // <-- ЗАМЕНА
    {"profiling_reset", "reset all stopwatches", cmd_profiling_reset},// <-- ЗАМЕНА
    {"led_enable", "turn LED on", cmd_led_enable},
    {"led_disable", "turn LED off", cmd_led_disable},
    {"led_blink", "blink LED", cmd_led_blink},
    {"led_period", "set LED period: led_period <period_ms>", cmd_led_period},
    {"button", "button state and presses", cmd_button},
    {"pi_start", "start pi calculation: pi_start [terms]", cmd_pi_start},
    {"pi", "get pi result or state", cmd_pi},
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);