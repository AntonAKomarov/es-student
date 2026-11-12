add_library(device STATIC src/device.c)
target_include_directories(device PUBLIC include)
target_link_libraries(device PRIVATE pico_stdlib pico_unique_id)
# Библиотека сама знает о типе платы
target_compile_definitions(device PUBLIC DEVICE_BOARD="${PICO_BOARD}")