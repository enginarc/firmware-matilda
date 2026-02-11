#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"
#include "driver/gpio.h"

// --- ST7789 Display (Hardware SPI) ---
#define LCD_HOST    SPI2_HOST
#define PIN_LCD_SCLK (gpio_num_t)3
#define PIN_LCD_MOSI (gpio_num_t)4
#define PIN_LCD_RST (gpio_num_t)5
#define PIN_LCD_DC  (gpio_num_t)6
#define PIN_LCD_CS       -1             // Tied to GND on most Super Mini modules
#define PIN_LCD_BL  (gpio_num_t)7  // PWM capable

#define LCD_H_RES        240
#define LCD_V_RES        320
#define LCD_PIXEL_CLOCK  (40 * 1000 * 1000) // 40MHz

// --- Relays (2-Channel Module) ---
#define PIN_RELAY_FAN (gpio_num_t)11  // Relay 2: Cooling Fan
#define PIN_RELAY_LED (gpio_num_t)12  // Relay 1: UV LEDs

// --- Input & Feedback ---
#define PIN_BUZZER       (gpio_num_t)13
#define PIN_RESERVED     (gpio_num_t)1

#define PIN_BTN_CNCL_RST   (gpio_num_t)2

#define PIN_TWT_SW       (gpio_num_t)9 // TWT Switch (Timer Nob click)
#define PIN_TWT_DIR1        (gpio_num_t)10
#define PIN_TWT_DIR2        (gpio_num_t)8

void board_init_gpio();

#ifdef __cplusplus
}
#endif