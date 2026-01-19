#include "board_config.h"
#include "esp_attr.h"
#include "driver/gpio.h"

// Declaration of the external adjustment function from the exposure component
extern "C" void exposure_adjust(int delta);

// ISR for the EC11 Encoder
static void IRAM_ATTR encoder_isr_handler(void* arg) {
    static int last_level_a = -1;
    int current_level_a = gpio_get_level(PIN_ENC_A);
    
    if (current_level_a != last_level_a) {
        last_level_a = current_level_a;
        if (current_level_a == 0) { // Falling edge
            if (gpio_get_level(PIN_ENC_B) == 0) {
                exposure_adjust(-5); // 5 second intervals (Flow 2)
            } else {
                exposure_adjust(5);  // 5 second intervals (Flow 2)
            }
        }
    }
}

void board_init_gpio(void) {
    // 1. Configure Outputs: Relays, Status LED, and Buzzer
    gpio_config_t io_conf_out = {};
    io_conf_out.intr_type = GPIO_INTR_DISABLE;
    io_conf_out.mode = GPIO_MODE_OUTPUT;
    io_conf_out.pin_bit_mask = (1ULL << PIN_RELAY_LED) | 
                               (1ULL << PIN_RELAY_FAN) | 
                               (1ULL << PIN_STATUS_LED)|
                               (1ULL << PIN_BUZZER);
    io_conf_out.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_out.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf_out);

    // Initial State: Turn OFF relays (High) and LEDs/Buzzer (Low)
    gpio_set_level(PIN_RELAY_LED, 1);
    gpio_set_level(PIN_RELAY_FAN, 1);
    gpio_set_level(PIN_STATUS_LED, 0);
    gpio_set_level(PIN_BUZZER, 0);

    // 2. Configure LCD Backlight
    gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LCD_BL, 1);

    // 3. Configure Inputs: Control Buttons and Encoder Switch
    gpio_config_t io_conf_in = {};
    io_conf_in.intr_type = GPIO_INTR_DISABLE;
    io_conf_in.mode = GPIO_MODE_INPUT;
    io_conf_in.pin_bit_mask = //(1ULL << PIN_BTN_START) | 
                              // (1ULL << PIN_BTN_PAUSE) | 
                               (1ULL << PIN_BTN_STOP)  | 
                              // (1ULL << PIN_BTN_RESET) |
                               (1ULL << PIN_ENC_SW);
    io_conf_in.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_in.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_in);

    // 4. Configure Encoder Rotation Pins (Interrupt Driven)
    gpio_config_t io_conf_enc = {};
    io_conf_enc.intr_type = GPIO_INTR_ANYEDGE; // Capture rotation edges
    io_conf_enc.mode = GPIO_MODE_INPUT;
    io_conf_enc.pin_bit_mask = (1ULL << PIN_ENC_A) | (1ULL << PIN_ENC_B);
    io_conf_enc.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_enc.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_enc);

    // 5. Install ISR Service and add handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_ENC_A, encoder_isr_handler, (void*) PIN_ENC_A);
}