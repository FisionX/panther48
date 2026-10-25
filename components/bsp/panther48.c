/** @file panther48.c
 *  @details Board resources and the connection between timer and LED.
 */
#include <stdbool.h>
#include "panther48.h"
#include "led_driver.h"
#include "timer_driver.h"
#include "esp_attr.h"
#include "esp_cpu.h"

// Panther status LED on GPIO5, active high.
#define LED_BIT 5u
// Select one timer ID (0-3); the driver chooses its group automatically.
#define LED_TIMER_ID 0u
static bool initialized;

// Connect timer events to the LED driver.
// This callback runs in the ISR: do not print, wait, or block.
static esp_err_t bsp_led_toggle(void)
{
    return led_driver_toggle();
}

// Prepare the LED and connect its callback to the selected timer.
esp_err_t bsp_init(void)
{
    // Check the calling CPU before accessing hardware.
    if (esp_cpu_get_core_id() != 0) {
        return ESP_ERR_INVALID_STATE;
    }
    if (initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Keep the LED off until timer setup is complete.
    esp_err_t ret = led_driver_init(LED_BIT);
    if (ret != ESP_OK) {
        return ret;
    }
    // Register the callback once. Start counting in a separate call.
    ret = timer_driver_init(LED_TIMER_ID, bsp_led_toggle);
    if (ret != ESP_OK) {
        return ret;
    }
    initialized = true;
    return ESP_OK;
}

// Start blinking with the interval given in microseconds.
esp_err_t bsp_led_blink_start(uint32_t interval_us)
{
    if (interval_us == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    return timer_driver_start_periodic(LED_TIMER_ID, interval_us);
}

// Stop the blink timer and turn the LED off.
esp_err_t bsp_led_blink_stop(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Stop callbacks before changing the LED from the main task.
    esp_err_t ret = timer_driver_stop(LED_TIMER_ID);
    if (ret != ESP_OK) {
        return ret;
    }
    return led_driver_set(false);
}

// Return the query status; write the saved timer error to *error.
esp_err_t bsp_led_blink_get_error(esp_err_t *error)
{
    if (error == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    return timer_driver_get_last_error(LED_TIMER_ID, error);
}
