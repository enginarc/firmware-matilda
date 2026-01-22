#include "board_config.h"
#include "driver/gpio.h"
#include "esp_attr.h"

// Declaration of the external adjustment function from the exposure component
extern "C" void exposure_adjust(int delta);

// ISR for the Thumbwheel Toggle (TWT) directions
static void IRAM_ATTR twt_isr_handler(void* arg) {
    gpio_num_t gpio_num = (gpio_num_t)(uint32_t)arg;
    
    // TWT is momentary; a falling edge indicates a toggle in that direction
    if (gpio_num == PIN_TWT_DIR1) {
        exposure_adjust(5);  // Increment timer by 5s
    } else if (gpio_num == PIN_TWT_DIR2) {
        exposure_adjust(-5); // Decrement timer by 5s
    }
}

void board_init_gpio(void) {
    // 1. Configure Outputs: Relays, Buzzer, and LCD Backlight
    gpio_config_t io_conf_out = {};
    io_conf_out.intr_type = GPIO_INTR_DISABLE;
    io_conf_out.mode = GPIO_MODE_OUTPUT;
    io_conf_out.pin_bit_mask = (1ULL << PIN_RELAY_LED) | 
                               (1ULL << PIN_RELAY_FAN) | 
                               (1ULL << PIN_BUZZER)    |
                               (1ULL << PIN_LCD_BL);
    io_conf_out.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_out.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf_out);

    // Initial State: Turn OFF relays (High) and Buzzer (Low)
    gpio_set_level(PIN_RELAY_LED, 1);
    gpio_set_level(PIN_RELAY_FAN, 1);
    gpio_set_level(PIN_BUZZER, 0);
    gpio_set_level(PIN_LCD_BL, 1); // Turn ON backlight

    // 2. Configure Inputs: TWT Switch and Cancel/Reset Button
    gpio_config_t io_conf_in = {};
    io_conf_in.intr_type = GPIO_INTR_DISABLE;
    io_conf_in.mode = GPIO_MODE_INPUT;
    io_conf_in.pin_bit_mask = (1ULL << PIN_TWT_SW) | 
                               (1ULL << PIN_BTN_CNCL_RST);
    io_conf_in.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_in.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_in);

    // 3. Configure TWT Direction Pins (Interrupt Driven)
    gpio_config_t io_conf_twt = {};
    io_conf_twt.intr_type = GPIO_INTR_NEGEDGE; // Trigger on press (GND connection)
    io_conf_twt.mode = GPIO_MODE_INPUT;
    io_conf_twt.pin_bit_mask = (1ULL << PIN_TWT_DIR1) | (1ULL << PIN_TWT_DIR2);
    io_conf_twt.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_twt.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_twt);

    // 4. Install ISR Service and add handlers
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_TWT_DIR1, twt_isr_handler, (void*) PIN_TWT_DIR1);
    gpio_isr_handler_add(PIN_TWT_DIR2, twt_isr_handler, (void*) PIN_TWT_DIR2);
}