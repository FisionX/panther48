#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#include "hal/gpio_types.h"
#include "mygpio.h"

#define SET_BIT(reg,x) (reg |= (1UL << (uint8_t)x))
#define UNSET_BIT(reg,x) (reg &= !(1UL << (uint8_t)x))
#define GET_BIT(reg,x) ((reg >> x) & 0x1))

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

[[maybe_unused]]static void set_input_gpio(uint32_t *reg)
{
    SET_BIT(*reg, FUN_IE);
    SET_BIT(*reg, FUN_WPU);
}

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
        SET_BIT(GPIO_ENABLE, 2);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(250));
        UNSET_BIT(GPIO_ENABLE, 2);
        ESP_LOGI(TAG, "LED OFF");
    }
}

void app_main(void)
{
    SET_BIT(GPIO_OUT, 2); // Config gpio 4 as output
    //gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    bar();
}
