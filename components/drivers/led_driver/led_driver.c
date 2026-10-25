/** @file led_driver.c
 *  @details LED state and control, independent of the timer.
 */
#include "led_driver.h"
#include "mygpio.h"
#include "esp_attr.h"

// Store the state of one LED. The BSP controls access to it.
static bool initialized;
static bool led_state;
static uint8_t led_pin;

// Set up the LED pin and leave it off.
esp_err_t led_driver_init(uint8_t pin)
{
    // Allow BSP setup to retry if interrupt allocation failed.
    if (initialized) {
        if (pin != led_pin) {
            return ESP_ERR_INVALID_STATE;
        }
        return ESP_OK;
    }
    // Call the mygpio API. Propagate errors without bypassing it.
    esp_err_t ret = mygpio_set_direction(pin, MYGPIO_MODE_OUTPUT);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = mygpio_digital_write(pin, false);
    if (ret != ESP_OK) {
        return ret;
    }
    led_pin = pin;
    led_state = false;
    initialized = true;
    return ESP_OK;
}

// Request the LED level through mygpio. This path is not IRAM-safe.
esp_err_t led_driver_set(bool on)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = mygpio_digital_write(led_pin, on);
    if (ret != ESP_OK) {
        return ret;
    }
    // Update the saved state only after the write succeeds.
    led_state = on;
    return ESP_OK;
}

// Toggle the current LED state.
esp_err_t led_driver_toggle(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Reuse set() to keep its checks and error handling.
    return led_driver_set(!led_state);
}
