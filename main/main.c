#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "board_config.h"
#include "store.h"
#include "exposure.h"
#include "display.h"

static const char *TAG = "MATILDA";


void app_main(void) {
    store_init();
    board_init_gpio();
    display_init();
    exposure_init();

    while (1) {
        // Flow 3 & 4: Start/Pause Knob Click
        if (gpio_get_level(PIN_ENC_SW) == 0) {
            if (exposure_get_state() == STATE_RUNNING) {
                exposure_pause();
            } else {
                exposure_start();
            }
            while(gpio_get_level(PIN_ENC_SW) == 0) vTaskDelay(10); // Debounce
        }

        // Flow 5 & 6: Cancel/Reset Button
        if (gpio_get_level(PIN_BTN_STOP) == 0) {
            if (exposure_get_state() == STATE_RUNNING) {
                exposure_stop(false); // Cancel
            } else {
                exposure_reset();     // Reset to last-duration
            }
            while(gpio_get_level(PIN_BTN_STOP) == 0) vTaskDelay(10); // Debounce
        }

        // Display Update Logic
        display_lock();
        exposure_state_t state = exposure_get_state();
        uint32_t timer = exposure_get_timer();
        
        // Update your LVGL labels here:
        // switch(state) {
        //    case STATE_PAUSED: lv_label_set_text(lbl_status, "Paused"); break;
        //    case STATE_FINISHED: lv_label_set_text(lbl_status, "Finished"); break;
        //    case STATE_CANCELLED: lv_label_set_text(lbl_status, "Cancelled"); break;
        //    default: lv_label_set_text(lbl_status, "");
        // }
        // lv_label_set_text_fmt(lbl_timer, "%02lu:%02lu", timer/60, timer%60);
        
        display_unlock();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}