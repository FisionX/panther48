/** @file application.c
 *  @details Application behavior: toggle the LED every second.
 */
#include <stdbool.h>
#include <stddef.h>
#include "application.h"
#include "panther.h"
#include "led_driver.h"
#include "mytimer.h"

// Select one timer ID (0-3); the driver chooses its group automatically.
#define LED_TIMER_ID 0u

static bool initialized;

// Connect timer events to the LED driver.
// This callback runs in the ISR: do not print, wait, or block.
static esp_err_t application_led_toggle(void)
{
    return led_driver_toggle();
}

// Set up the board and start toggling the LED every second.
esp_err_t application_init(void)
{
    if (initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Prepare the board before starting the timer.
    esp_err_t ret = panther_init();
    if (ret != ESP_OK) {
        return ret;
    }
    ret = mytimer_init(LED_TIMER_ID, application_led_toggle);
    if (ret != ESP_OK) {
        return ret;
    }
    initialized = true;
    // Toggle every second: 1 second ON, then 1 second OFF.
    // The application chooses the interval; the BSP chooses the LED pin.
    return application_led_blink_start(1000000UL);
}

// Start blinking with the interval given in microseconds.
esp_err_t application_led_blink_start(uint32_t interval_us)
{
    if (interval_us == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (initialized == false) {
        return ESP_ERR_INVALID_STATE;
    }
    return mytimer_start_periodic(LED_TIMER_ID, interval_us);
}

// Stop the blink timer and turn the LED off.
esp_err_t application_led_blink_stop(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Stop callbacks before changing the LED from the main task.
    esp_err_t ret = mytimer_stop(LED_TIMER_ID);
    if (ret != ESP_OK) {
        return ret;
    }
    return led_driver_set(false);
}

// Return the query status; write the saved timer error to *error.
esp_err_t application_led_blink_get_error(esp_err_t *error)
{
    if (error == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (initialized == false) {
        return ESP_ERR_INVALID_STATE;
    }
    return mytimer_get_last_error(LED_TIMER_ID, error);
}
