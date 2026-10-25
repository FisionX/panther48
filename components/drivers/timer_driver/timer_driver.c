/** @file timer_driver.c
 *  @details Four independent timers. The ISR receives the timer context as its argument.
 */
#include <stddef.h>
#include <stdbool.h>
#include "timer_driver.h"
#include "timer_hal.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_intr_alloc.h"
#include "soc/interrupts.h"

typedef struct {
    unsigned timer_id;
    intr_handle_t intr;
    timer_driver_callback_t callback;
    bool initialized;
    volatile esp_err_t last_error;
} timer_context_t;

// Keep each timer context in internal RAM so the ISR can access it without flash cache.
static DRAM_ATTR timer_context_t timers[4];

// Use this table only during setup, not inside the ISR.
// Entries follow timer IDs 0, 1, 2, 3.
static const int interrupt_sources[4] = {
    ETS_TG0_T0_LEVEL_INTR_SOURCE, ETS_TG0_T1_LEVEL_INTR_SOURCE,
    ETS_TG1_T0_LEVEL_INTR_SOURCE, ETS_TG1_T1_LEVEL_INTR_SOURCE
};

// Check the timer ID and calling CPU.
static esp_err_t timer_driver_validate(unsigned timer_id)
{
    // Validate before indexing arrays or accessing registers.
    if (timer_id > 3) {
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_cpu_get_core_id() != 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

// ESP-IDF requires a void ISR. Its argument identifies the timer.
// Clear the interrupt, run the callback, and rearm the alarm.
static void IRAM_ATTR timer_driver_isr(void *arg)
{
    timer_context_t *ctx = arg;
    esp_err_t ret = hal_timer_ack(ctx->timer_id);
    if (ret != ESP_OK) {
        ctx->last_error = ret;
        return;
    }
    ret = ctx->callback();
    if (ret != ESP_OK) {
        // Stop only this timer if its callback fails. Other timers keep running.
        ctx->last_error = ret;
        ret = hal_timer_stop(ctx->timer_id);
        if (ret != ESP_OK) {
            ctx->last_error = ret;
        }
        return;
    }
    // Auto-reload resets the counter; the alarm still needs to be rearmed.
    ret = hal_timer_rearm(ctx->timer_id);
    if (ret != ESP_OK) {
        ctx->last_error = ret;
    }
}

// Prepare the selected timer and register its callback.
esp_err_t timer_driver_init(unsigned timer_id, timer_driver_callback_t callback)
{
    esp_err_t ret = timer_driver_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // Keep driver state indexed by ID; hardware mapping belongs to the HAL.
    timer_context_t *ctx = &timers[timer_id];
    if (ctx->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ret = hal_timer_init(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    ctx->timer_id = timer_id;
    ctx->callback = callback;
    // Register once, initially disabled. The context remains valid after this call.
    ret = esp_intr_alloc(interrupt_sources[timer_id],
                         ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL1 |
                         ESP_INTR_FLAG_INTRDISABLED,
                         timer_driver_isr, ctx, &ctx->intr);
    if (ret != ESP_OK) {
        ctx->callback = NULL;
        return ret;
    }
    ctx->initialized = true;
    return ESP_OK;
}

// Start or restart the selected timer with the requested interval.
esp_err_t timer_driver_start_periodic(unsigned timer_id, uint32_t period_us)
{
    esp_err_t ret = timer_driver_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    if (period_us == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    // Keep driver state indexed by ID; hardware mapping belongs to the HAL.
    timer_context_t *ctx = &timers[timer_id];
    if (!ctx->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Disable this timer ISR while changing its configuration.
    ret = esp_intr_disable(ctx->intr);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = hal_timer_stop(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = hal_timer_configure(timer_id, period_us);
    if (ret != ESP_OK) {
        return ret;
    }
    ctx->last_error = ESP_OK;
    // Enable the CPU interrupt while the timer is still stopped and has no pending event.
    ret = esp_intr_enable(ctx->intr);
    if (ret != ESP_OK) {
        return ret;
    }
    return hal_timer_start(timer_id);
}

// Stop only the selected timer and disable its interrupt.
esp_err_t timer_driver_stop(unsigned timer_id)
{
    esp_err_t ret = timer_driver_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    // Keep driver state indexed by ID; hardware mapping belongs to the HAL.
    timer_context_t *ctx = &timers[timer_id];
    if (!ctx->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ret = esp_intr_disable(ctx->intr);
    if (ret != ESP_OK) {
        return ret;
    }
    return hal_timer_stop(timer_id);
}

// Copy the selected timer error to the output pointer.
esp_err_t timer_driver_get_last_error(unsigned timer_id, esp_err_t *error)
{
    esp_err_t ret = timer_driver_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    if (error == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // Keep driver state indexed by ID; hardware mapping belongs to the HAL.
    timer_context_t *ctx = &timers[timer_id];
    if (!ctx->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    // Reading the error does not clear it or wait for another interrupt.
    *error = ctx->last_error;
    return ESP_OK;
}
