#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of the UV system states
 */
typedef enum {
    STATE_IDLE,      // Waiting for user to start
    STATE_RUNNING,   // LEDs and Fan are ON, timer counting down
    STATE_PAUSED,    // Countdown frozen, LEDs OFF, Fan remains ON
    STATE_FINISHED,  // Countdown reached zero
    STATE_CANCELLED  // User manually stopped the process
} exposure_state_t;

/**
 * @brief Initialize the exposure logic, load last duration, and setup timers
 */
void exposure_init(void);

/**
 * @brief Start or resume the exposure cycle
 */
void exposure_start(void);

/**
 * @brief Pause the active exposure cycle
 */
void exposure_pause(void);

/**
 * @brief Stop the exposure cycle
 * @param finished True if stopped due to timer reaching zero, false if cancelled
 */
void exposure_stop(bool finished);

/**
 * @brief Adjust the countdown timer
 * @param delta Seconds to add or subtract (e.g., +10 or -10)
 */
void exposure_adjust(int delta);

/**
 * @brief Get the current countdown timer value
 * @return Remaining seconds
 */
uint32_t exposure_get_timer(void);

/**
 * @brief Get the current system state
 * @return exposure_state_t
 */
exposure_state_t exposure_get_state(void);

#ifdef __cplusplus
}
#endif