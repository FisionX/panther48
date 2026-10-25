#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

// One active-high LED, owned by the BSP.
// Set up the pin and leave it off. Repeating setup with the same pin is allowed.
esp_err_t led_driver_init(uint8_t pin);
// Set and save the LED state. Do not call from a task while the ISR can change it.
esp_err_t led_driver_set(bool on);
// Toggle the saved state. This driver does not depend on a timer.
esp_err_t led_driver_toggle(void);
