#include "exposure.h"
#include "board_config.h"
#include "store.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static exposure_state_t current_state = STATE_IDLE;
static uint32_t current_timer = 60;
static uint32_t fan_timer_sec = 0; // Tracks 10s overrun
static esp_timer_handle_t countdown_timer;

static void timer_callback(void* arg) {
    // 1. Manage Fan Overrun Logic
    if (current_state == STATE_RUNNING) {
        gpio_set_level(PIN_RELAY_FAN, 0); // Ensure Fan is ON
        fan_timer_sec = 0; // Reset overrun timer while running
        
        // 2. Countdown Logic
        if (current_timer > 0) {
            current_timer--;
        } else {
            exposure_stop(true); // Transitions to STATE_FINISHED
        }
    } else if (fan_timer_sec > 0) {
        fan_timer_sec--;
        if (fan_timer_sec == 0) {
            gpio_set_level(PIN_RELAY_FAN, 1); // Fan OFF after delay
        }
    }
}

void exposure_init() {
    // Load last duration from NVS (Flow 1)
    current_timer = store_get_u32("last-duration", 60);

    // Flow 1: Power-on Self Test (3 seconds)
    gpio_set_level(PIN_RELAY_LED, 0);
    gpio_set_level(PIN_RELAY_FAN, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
    gpio_set_level(PIN_RELAY_LED, 1);
    gpio_set_level(PIN_RELAY_FAN, 1);
    
    current_state = STATE_IDLE;
    
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "countdown"
    };
    esp_timer_create(&timer_args, &countdown_timer);
    esp_timer_start_periodic(countdown_timer, 1000000); // 1 second
}

void exposure_start() {
    // Flow 3: Start/Resume
    if (current_state == STATE_IDLE || current_state == STATE_PAUSED) {
        current_state = STATE_RUNNING;
        gpio_set_level(PIN_RELAY_LED, 0);
        gpio_set_level(PIN_RELAY_FAN, 0);
        
        // Save as last-duration in NVS on manual start
        store_set_u32("last-duration", current_timer);
        fan_timer_sec = 0;
    }
}

void exposure_pause() {
    // Flow 4: Pause
    if (current_state == STATE_RUNNING) {
        current_state = STATE_PAUSED;
        gpio_set_level(PIN_RELAY_LED, 1);
        fan_timer_sec = 10; // Fan stays ON for 10s
    } else if (current_state == STATE_PAUSED) {
        exposure_start(); // Toggle behavior
    }
}

void exposure_stop(bool finished) {
    // Flow 5 & 7: Cancel or Finish
    current_state = finished ? STATE_FINISHED : STATE_CANCELLED;
    gpio_set_level(PIN_RELAY_LED, 1);
    fan_timer_sec = 10; // Fan OFF after 10s
}

void exposure_reset() {
    // Flow 6: Reset to last duration
    if (current_state != STATE_RUNNING) {
        current_state = STATE_IDLE;
        current_timer = store_get_u32("last-duration", 60);
        gpio_set_level(PIN_RELAY_LED, 1);
        fan_timer_sec = 10; 
    }
}

void exposure_adjust(int delta) {
    // Flow 2: Adjust in 5s intervals (handled by delta from ISR)
    if ((int)current_timer + delta >= 0) {
        current_timer += delta;
    } else {
        current_timer = 0;
    }
}

uint32_t exposure_get_timer() { return current_timer; }
exposure_state_t exposure_get_state() { return current_state; }