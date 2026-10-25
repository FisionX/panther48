/** @file timer_driver.h
 *  @details Timer API for TG0/T0, TG0/T1, TG1/T0, and TG1/T1 on classic ESP32.
 */
#pragma once
#include <stdint.h>
#include "esp_err.h"

// The callback runs in the ISR. Use IRAM code and internal data; do not block or print.
typedef esp_err_t (*timer_driver_callback_t)(void);

// Timer IDs: 0 = TG0/T0, 1 = TG0/T1, 2 = TG1/T0, 3 = TG1/T1.
// The driver selects the group automatically. Other IDs are rejected.
// Call control functions from one task on CPU0, never from an ISR.
// Prepare the selected timer and register its callback.
esp_err_t timer_driver_init(unsigned timer_id, timer_driver_callback_t callback);
// Start or restart the selected timer with the requested interval.
esp_err_t timer_driver_start_periodic(unsigned timer_id, uint32_t period_us);
// Stop only the selected timer and disable its interrupt.
esp_err_t timer_driver_stop(unsigned timer_id);
// Return the query status. Write the selected timer error to *error.
esp_err_t timer_driver_get_last_error(unsigned timer_id, esp_err_t *error);
