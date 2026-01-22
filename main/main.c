#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "board_config.h"
#include "display.h"
#include "store.h"
#include "exposure.h"

void app_main(void) {
    // Initialize system components
    store_init();
    board_init_gpio();
    display_init();
    exposure_init();

    while (1) {
        // Toggle Start/Pause using the Thumbwheel click (PIN_TWT_SW)
        if (gpio_get_level(PIN_TWT_SW) == 0) {
            if (exposure_get_state() == STATE_RUNNING) {
                exposure_pause();
            } else {
                exposure_start();
            }
            // Debounce: wait for release
            while(gpio_get_level(PIN_TWT_SW) == 0) vTaskDelay(pdMS_TO_TICKS(10));
        }

        // Cancel or Reset using the dedicated button (PIN_BTN_CNCL_RST)
        if (gpio_get_level(PIN_BTN_CNCL_RST) == 0) {
            if (exposure_get_state() == STATE_RUNNING) {
                exposure_stop(false); // Cancel active exposure
            } else {
                exposure_reset();     // Reset timer to last-used duration
            }
            // Debounce: wait for release
            while(gpio_get_level(PIN_BTN_CNCL_RST) == 0) vTaskDelay(pdMS_TO_TICKS(10));
        }

        // Periodically refresh the display (logic handled in display component)
        display_lock();
        // UI rendering logic here
        display_unlock();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}