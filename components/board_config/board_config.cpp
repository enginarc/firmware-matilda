#include "board_config.h"

void board_init_gpio() {
    // Configure Relay Pins as Outputs
    gpio_config_t relay_cfg = {
        .pin_bit_mask = (1ULL << PIN_RELAY_LED) | (1ULL << PIN_RELAY_FAN) | (1ULL << PIN_STATUS_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&relay_cfg);

    // Relays are often Active-Low on these modules, set to HIGH (OFF) initially
    gpio_set_level(PIN_RELAY_LED, 1);
    gpio_set_level(PIN_RELAY_FAN, 1);
    gpio_set_level(PIN_STATUS_LED, 0); // Internal LED OFF

    // Configure Start Button
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << PIN_BUTTON_START),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&btn_cfg);
}