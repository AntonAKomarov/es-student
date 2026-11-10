#pragma once
#include "pico/unique_id.h"
#include <stdint.h>

// Задаем новые имена по заданию
#define DEVICE_NAME "231-firmware"
#define FIRMWARE_VERSION "1.0.0"
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/AntonAKomarov/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

void device_info(void);



   