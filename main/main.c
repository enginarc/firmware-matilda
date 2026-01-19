#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "board_config.h"
#include "store.h"
#include "exposure.h"
#include "display.h"

static const char *TAG = "UV_BOX";

// void start_exposure_cycle(uint32_t duration_sec) {
//     ESP_LOGI(TAG, "Starting Cycle: Fan ON, Cooling stage...");
    
//     // 1. Pre-exposure: Turn on Fan for 2 seconds to stabilize airflow
//     gpio_set_level(PIN_RELAY_FAN, 0); // Active Low
//     vTaskDelay(pdMS_TO_TICKS(2000));

//     // 2. Exposure: Turn on UV LEDs
//     ESP_LOGI(TAG, "Lamps Active for %lu seconds", duration_sec);
//     gpio_set_level(PIN_RELAY_LED, 0); // Active Low
//     gpio_set_level(PIN_STATUS_LED, 1); // Blue indicator ON

//     vTaskDelay(pdMS_TO_TICKS(duration_sec * 1000));

//     // 3. Finish: Turn off UV LEDs
//     gpio_set_level(PIN_RELAY_LED, 1);
//     gpio_set_level(PIN_STATUS_LED, 0);
//     ESP_LOGI(TAG, "Exposure Finished. Cooldown starting...");

//     // 4. Cooldown: Keep fan running for 10 seconds to dump heat
//     vTaskDelay(pdMS_TO_TICKS(10000));
//     gpio_set_level(PIN_RELAY_FAN, 1);
    
//     ESP_LOGI(TAG, "Cycle Complete. System Ready.");
// }

void app_main(void) {
    store_init();
    // 1. Hardware Init
    board_init_gpio();
    
    // 2. Display Init (Refactored)
    if (display_init() != ESP_OK) {
        ESP_LOGE(TAG, "Display initialization failed!");
        return;
    }

    exposure_init();

    // 3. Initial UI Setup
    display_lock();
    
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0); // Black background

    lv_obj_t *ui_title = lv_label_create(scr);
    lv_label_set_text(ui_title, "UV SYSTEM");
    lv_obj_set_style_text_font(ui_title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ui_title, lv_palette_main(LV_PALETTE_AMBER), 0);
    lv_obj_align(ui_title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *ui_status = lv_label_create(scr);
    lv_label_set_text(ui_status, "IDLE - READY");
    lv_obj_align(ui_status, LV_ALIGN_CENTER, 0, 0);
    
    display_unlock();

    ESP_LOGI(TAG, "UV Exposure Box Initialized (ESP32-S3 Super Mini)");

while (1) {
        
        if (gpio_get_level(PIN_BTN_START) == 0) exposure_start();
        if (gpio_get_level(PIN_BTN_PAUSE) == 0) exposure_pause();
        if (gpio_get_level(PIN_BTN_STOP) == 0) exposure_stop(false);
        if (gpio_get_level(PIN_BTN_RESET) == 0) exposure_reset();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}