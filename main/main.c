/**
 * @file main.c
 * @details Main file containing the entry point of the program
 * @author Fernando R Jacobo & Alejandro Rocha
 */
#include "application.h"
#include "esp_log.h"
#include "sdkconfig.h"

// The interrupt is allocated on the CPU that runs initialization.
#if !CONFIG_ESP_MAIN_TASK_AFFINITY_CPU0
#error "Selecciona ESP_MAIN_TASK_AFFINITY_CPU0=y."
#endif

static const char *TAG = "main";

// ESP-IDF requires a void entry point. Report startup errors here.
void app_main(void)
{
    // Let the application choose the behavior; no pin or register access here.
    esp_err_t ret = application_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start LED: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "LED: 1 second ON, 1 second OFF");
    // The ISR keeps running after this function returns. No busy wait is needed.
}
