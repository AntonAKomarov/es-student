#include "stdio-text-protocol/stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t line_len = 0;
static command_t cmd;

void stdio_text_protocol_init(void) {
    line_len = 0;
    cmd.truncated = false;
}

const command_t *stdio_text_protocol_handle(void) {
    int symbol = getchar_timeout_us(0);
    
    if (symbol == PICO_ERROR_TIMEOUT) {
        return NULL;
    }

    // Конец строки
    if (symbol == '\r' || symbol == '\n') {
        putchar('\n');
        if (line_len == 0) {
            return NULL; // пустую строку пропускаем
        }
        line[line_len] = '\0';

        // Деление строки на слова
        cmd.argc = 0;
        cmd.name = NULL;
        bool was_truncated = cmd.truncated; 
        bool in_word = false;
        char *word_start = NULL;
        int word_count = 0;

        for (uint32_t i = 0; i <= line_len; i++) {
            if (line[i] == ' ' || line[i] == '\0') {
                if (in_word) {
                    if (line[i] == ' ') {
                        line[i] = '\0';
                    }
                    in_word = false;
                    word_count++;
                    if (word_count == 1) {
                        cmd.name = word_start;
                    } else if (word_count <= 5) {
                        if (cmd.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS) {
                            cmd.argv[cmd.argc++] = word_start;
                        }
                    } else {
                        was_truncated = true;
                    }
                }
            } else if (!in_word) {
                in_word = true;
                word_start = &line[i];
            }
        }
        
        cmd.truncated = was_truncated;
        line_len = 0;
        return &cmd;
    }

    // Backspace (0x08 или 0x7f)
    if (symbol == 0x08 || symbol == 0x7F) {
        if (line_len > 0) {
            line_len--;
            printf("\b \b");
            if (line_len < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1) {
                cmd.truncated = false;
            }
        }
        return NULL;
    }

    // Печатный символ
    if (symbol >= 32 && symbol <= 126) {
        if (line_len < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1) {
            line[line_len++] = (char)symbol;
            putchar(symbol);
        } else {
            cmd.truncated = true;
        }
    }
    
    return NULL;
}