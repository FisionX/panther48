/**
 * @details Example project demonstration for custom made
 * gpio library
 */
// #include <stdio.h>
// #include <stdint.h>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "driver/gpio.h"

// #include "freertos/projdefs.h"
// #include "hal/gpio_types.h"
// #include "mygpio.h"

// #define BTN1 18
// #define BTN2 19

// #define LED_BIT 2
// #define BTN_BIT BTN1
// #define BTN1 18
// #define BTN2 19

// static const char *TAG = "main";

// typedef struct BUTTON_STATE {
//     bool button1;
//     bool button2;
// } button_state_t;

// [[maybe_unused]] static esp_err_t config_gpio(void)
// {
//     esp_err_t ret = ESP_OK;
//     ret = mygpio_set_direction(LED_BIT, MYGPIO_MODE_OUTPUT);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to configure led");
//         return ret;
//     }

//     ret = mygpio_set_direction(BTN_BIT, MYGPIO_MODE_INPUT);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to configure btn");
//         return ret;
//     }

//     return ret;
// }

// [[maybe_unused]] static void foo(void)
// {
//     bool btn_state;

//     while (true) {
//         mygpio_get_level(BTN_BIT, &btn_state);
//         if (!btn_state) {
//             ESP_LOGI(TAG, "BTN pressed");
//         }

//         vTaskDelay(pdMS_TO_TICKS(250));
//     }
// }

// [[maybe_unused]] static void bar(void)
// {
//     while (true) {
//         vTaskDelay(pdMS_TO_TICKS(250));
//         if (mygpio_digital_write(LED_BIT, true) != ESP_OK) {
//             ESP_LOGE(TAG, "Failed to turn on led");
//         }

//         ESP_LOGI(TAG, "LED ON");

//         vTaskDelay(pdMS_TO_TICKS(250));
//         if (mygpio_digital_write(LED_BIT, false) != ESP_OK) {
//             ESP_LOGE(TAG, "Failed to turn off led");
//         }

//         ESP_LOGI(TAG, "LED OFF");
//     }
// }

// void app_main(void)
// {
//     esp_err_t ret = ESP_OK;
//     ret = config_gpio();
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to config gpio");
//         return;
//     }
//     //gpio_set_direction(GPIO_NUM_18, GPIO_MODE_DEF_INPUT);
//     //gpio_set_pull_mode(GPIO_NUM_18, GPIO_PULLUP_ONLY);

//     foo();
//     //bar();

//     return;
// }
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
