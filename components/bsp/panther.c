#include <stdbool.h>
#include "panther.h"
#include "led_driver.h"
#include "esp_log.h"
#include "esp_cpu.h"

// Panther status LED on GPIO5, active high.
#define LED_BIT 5u
static char* TAG = "panther";
static bool initialized;

esp_err_t panther_init(void)
{
    // Check the calling CPU before accessing hardware.
    if (esp_cpu_get_core_id() != 0) {
        return ESP_ERR_INVALID_STATE;
    }
    if (initialized) {
        ESP_LOGE(TAG, "bsp is already initialized");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = led_driver_init(LED_BIT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not start led driver");
        return ret;
    }
    initialized = true;
    return ESP_OK;
}
