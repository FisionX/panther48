#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#include "mygpio.h"


#define _BIT(X) (1UL<<X)
#define BTN1 18
#define BTN2 19

#define LED_BIT 2
#define LED_BIT2 4
#define BTN1 18
#define BTN2 19

static const char *TAG = "main";

typedef struct BUTTON_STATE {
    bool button1;
    bool button2;
} button_state_t;

//static void foo(void)
//{
//    while (true) {
//        state.button1 = GET_BIT(GPIO_IN, BUTTON1);
//        state.button2 = GET_BIT(GPIO_IN, BUTTON2);
//        if (!state.button1 && prev_state.button1) {
//            ESP_LOGI(TAG, "Led encendido");
//        }
//
//        if (!state.button2 && prev_state.button2) {
//            ESP_LOGI(TAG, "Led Apagado");
//        }
//
//        prev_state = state;
//        vTaskDelay(pdMS_TO_TICKS(20));
//    }
//}

static void bar(void)
{
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(250));
        mygpio_digital_write(2, true);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(250));
        mygpio_digital_write(2, false);
        ESP_LOGI(TAG, "LED OFF");
    }
}

void app_main(void)
{
    mygpio_set_direction(2, MYGPIO_MODE_OUTPUT);

    //bar();

    return;
}
