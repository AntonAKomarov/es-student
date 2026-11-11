#include "api-commands.h"
#include "device.h"
#include "profiling.h"
#include "led-task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <pico/stdlib.h>

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
    device_info();
}

static void cmd_uptime(const command_t *command) {
    (void)command;
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}

static void cmd_main_time_exec(const command_t *command) {
    (void)command;
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

static void cmd_main_time_reset(const command_t *command) {
    (void)command;
    profiling_reset_max();
    printf("Max reset\n");
}

static double calc_pi(uint32_t terms) {
    double sum = 0.0;
    double k = 1.0;
    for (uint32_t i = 0; i < terms; i++) {
        sum += k / (2.0 * i + 1.0);
        k = -k;
    }
    return sum * 4.0;
}

static void cmd_calc_pi(const command_t *command) {
    uint32_t terms = 1000000; 
    
    if (command->argc > 1) {
        printf("error: usage calc_pi [terms]\n");
        return;
    }
    if (command->argc == 1) {
        if (!parse_u32(command->argv[0], &terms) || terms == 0) {
            printf("error: usage calc_pi [terms]\n");
            return;
        }
    }
    
    uint64_t start_us = time_us_64();
    double result = calc_pi(terms);
    uint64_t spent_us = time_us_64() - start_us;
    
    printf("pi: %.9f\n", result);
    printf("time: %llu ms\n", spent_us / 1000);
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

// Таблица команд
const api_command_t api_commands[] = {
    {"info", "device passport", cmd_info},
    {"uptime", "time since reset", cmd_uptime},
    {"calc_pi", "calculate pi: calc_pi [terms]", cmd_calc_pi},
    {"main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec},
    {"main_time_reset", "reset maximum iteration", cmd_main_time_reset},
    {"led_enable", "turn LED on", cmd_led_enable},
    {"led_disable", "turn LED off", cmd_led_disable},
    {"led_blink", "blink LED", cmd_led_blink},
    {"led_period", "set LED period: led_period <period_ms>", cmd_led_period}
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);