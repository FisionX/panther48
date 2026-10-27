/**
 * @file application.h
 * @brief Application initialization and periodic LED blinking control.
 */
#pragma once
#include <stdint.h>
#include "esp_err.h"

// Set up the board and start toggling the LED every second. Return any error.
esp_err_t application_init(void);
// Start or restart blinking. The interval is in microseconds.
esp_err_t application_led_blink_start(uint32_t interval_us);
// Stop the timer and turn the LED off.
esp_err_t application_led_blink_stop(void);
// Read the last timer error through the output pointer.
esp_err_t application_led_blink_get_error(esp_err_t *error);
