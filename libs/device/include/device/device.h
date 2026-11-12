#pragma once
#include <stdint.h>

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Структура со сведениями о плате
typedef struct {
    char board[32];
    char serial[17];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char sdk_version[16];
} device_info_t;

// Заполняет структуру по указателю, ничего не печатает
void device_get_info(device_info_t *info);



   