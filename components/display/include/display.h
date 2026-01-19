#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "lvgl.h"
#include "esp_lcd_panel_ops.h"

/**
 * @brief Initialize ST7789 via SPI and start LVGL task
 */
esp_err_t display_init();

/**
 * @brief Thread-safe locking for LVGL operations
 */
void display_lock();
void display_unlock();

#ifdef __cplusplus
}
#endif