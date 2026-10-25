#pragma once
#include <stdint.h>
#include "esp_err.h"

// Prepare the LED and its timer callback from the owning task on CPU0.
esp_err_t bsp_init(void);
// Start or restart blinking. The interval is in microseconds.
esp_err_t bsp_led_blink_start(uint32_t interval_us);
// Stop the timer and turn the LED off.
esp_err_t bsp_led_blink_stop(void);
// Read the last timer error through the output pointer.
esp_err_t bsp_led_blink_get_error(esp_err_t *error);
