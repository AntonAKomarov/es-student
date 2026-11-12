#pragma once
#include "pico/unique_id.h"
#include <stdint.h>

#define DEVICE_NAME "231-firmware"
#define FIRMWARE_VERSION "1.0.0"
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/AntonAKomarov/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Структура со сведениями о плате
typedef struct {
    char board[32];
    char serial[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char sdk_version[16];
} device_info_t;

// Заполняет структуру по указателю, ничего не печатает
void device_get_info(device_info_t *info);



   