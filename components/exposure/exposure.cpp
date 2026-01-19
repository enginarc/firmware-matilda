#include "exposure.h"
#include "board_config.h"
#include "store.h"
#include "esp_timer.h"
#include "esp_log.h"

static exposure_state_t current_state = STATE_IDLE;
static uint32_t current_timer = 60;
static esp_timer_handle_t countdown_timer;

static void timer_callback(void* arg) {
    if (current_state == STATE_RUNNING) {
        if (current_timer > 0) {
            current_timer--;
        } else {
            exposure_stop(true); // Auto-finish
        }
    }
}

void exposure_init() {
    current_timer = store_get_duration();
    
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "countdown"
    };
    esp_timer_create(&timer_args, &countdown_timer);
    esp_timer_start_periodic(countdown_timer, 1000000); // 1 second
}

void exposure_start() {
    if (current_state == STATE_IDLE || current_state == STATE_PAUSED) {
        current_state = STATE_RUNNING;
        gpio_set_level(PIN_RELAY_LED, 0); // Active Low
        gpio_set_level(PIN_RELAY_FAN, 0);
        store_set_duration(current_timer);
    }
}

void exposure_pause() {
    if (current_state == STATE_RUNNING) {
        current_state = STATE_PAUSED;
        gpio_set_level(PIN_RELAY_LED, 1); // LEDs OFF
    } else if (current_state == STATE_PAUSED) {
        exposure_start(); // Resume
    }
}

void exposure_stop(bool finished) {
    current_state = finished ? STATE_FINISHED : STATE_CANCELLED;
    gpio_set_level(PIN_RELAY_LED, 1);
    gpio_set_level(PIN_RELAY_FAN, 1);
}

void exposure_adjust(int delta) {
    // Allows adjustment anytime (even during exposure per your workflow)
    if ((int)current_timer + delta >= 0) {
        current_timer += delta;
    }
}

void exposure_reset() {
    current_state = STATE_IDLE;
    current_timer = store_get_duration();
}

uint32_t exposure_get_timer() { return current_timer; }
exposure_state_t exposure_get_state() { return current_state; }