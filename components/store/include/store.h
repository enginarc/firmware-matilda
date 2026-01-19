#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the NVS flash storage
 * @return ESP_OK on success
 */
esp_err_t store_init(void);

/**
 * @brief Get a uint32 value from NVS
 * @param key The key string
 * @param default_val Value to return if key is not found
 * @return The stored value or default_val
 */
uint32_t store_get_u32(const char* key, uint32_t default_val);

/**
 * @brief Set a uint32 value in NVS
 * @param key The key string
 * @param value The value to save
 */
void store_set_u32(const char* key, uint32_t value);

uint32_t store_get_duration();
void store_set_duration(uint32_t val) ;

#ifdef __cplusplus
}
#endif