/** 
 * @file hal_timer.h
 *
 * @details Private API for hal_timer.c 
 * Call init before using.
 * Configure timer when it's stopped.
 *
 * @author Alejandro Rocha R.
 */

#pragma once
#ifndef TIMER_HAL_H
#define TIMER_HAL_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_attr.h"

// Timer IDs: 0 = TG0/T0, 1 = TG0/T1, 2 = TG1/T0, 3 = TG1/T1.
// The HAL checks the ID and selects the hardware group and timer.
// The driver handles setup order, exclusive ownership, and CPU0 checks.

// Enable the group clock and stop the selected timer.
esp_err_t hal_timer_init(unsigned timer_id);

// The timer must be stopped. Set a nonzero interval in microseconds and reset the counter.
esp_err_t hal_timer_configure(unsigned timer_id, uint32_t period_us);

// Enable counting, the alarm, and the selected timer interrupt.
esp_err_t hal_timer_start(unsigned timer_id);

// The following three functions are safe in an ISR.
// Stop the selected timer and clear its pending interrupt.
esp_err_t hal_timer_stop(unsigned timer_id);

// Clear the selected timer interrupt.
esp_err_t hal_timer_ack(unsigned timer_id);

// Enable the selected timer alarm again.
esp_err_t hal_timer_rearm(unsigned timer_id);

#endif