#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"
#include "driver/gpio.h"

// --- ST7789 Display (Hardware SPI) ---
#define LCD_HOST    SPI2_HOST
#define PIN_LCD_SCLK (gpio_num_t)12
#define PIN_LCD_MOSI (gpio_num_t)11
#define PIN_LCD_RST (gpio_num_t)10
#define PIN_LCD_DC  (gpio_num_t)9
#define PIN_LCD_CS       -1             // Tied to GND on most Super Mini modules
#define PIN_LCD_BL  (gpio_num_t)1  // PWM capable

#define LCD_H_RES        240
#define LCD_V_RES        320
#define LCD_PIXEL_CLOCK  (40 * 1000 * 1000) // 40MHz

// --- Relays (2-Channel Module) ---
#define PIN_RELAY_LED (gpio_num_t)2  // Relay 1: UV LEDs
#define PIN_RELAY_FAN (gpio_num_t)4  // Relay 2: Cooling Fan

// --- Input & Feedback ---
#define PIN_BUTTON_START (gpio_num_t)5
#define PIN_BUZZER       (gpio_num_t)3
#define PIN_STATUS_LED   (gpio_num_t)8 // Onboard Blue LED

void board_init_gpio();

#ifdef __cplusplus
}
#endif