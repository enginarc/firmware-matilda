#include "store.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "STORE";
static const char *NVS_NAMESPACE = "uv_storage";

esp_err_t store_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

uint32_t store_get_u32(const char* key, uint32_t default_val) {
    nvs_handle_t handle;
    uint32_t value = default_val;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        nvs_get_u32(handle, key, &value);
        nvs_close(handle);
    }
    return value;
}

void store_set_u32(const char* key, uint32_t value) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        nvs_set_u32(handle, key, value);
        nvs_commit(handle);
        nvs_close(handle);
    }
}