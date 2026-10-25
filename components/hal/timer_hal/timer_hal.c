/** 
 * @file hal_timer.c
 * 
 * @details Access to timer registries.
 * 
 * @author Alejandro Rocha R.
 */

#include "timer_hal.h"
#include "regs/timer_regs.h"

#include <stdbool.h>
#include "esp_private/periph_ctrl.h"
#include "xtensa/xtruntime.h"

// Enable each group clock once. Do not reset the group or its watchdogs.
static bool group_enabled[2];

// Check the ID before indexing arrays or accessing registers.
static esp_err_t IRAM_ATTR hal_timer_validate(unsigned timer_id)
{
    if (timer_id > 3) {
        return ESP_ERR_INVALID_ARG;
    }
    
    return ESP_OK;
}

// Enable the group clock and stop the selected timer.
esp_err_t hal_timer_init(unsigned timer_id)
{
    esp_err_t ret = hal_timer_validate(timer_id);
    
    if (ret != ESP_OK) {
        return ret;
    }

    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    if (!group_enabled[group]) {
        if (group == 0) {
            periph_module_enable(PERIPH_TIMG0_MODULE);
        }
        if (group == 1) {
            periph_module_enable(PERIPH_TIMG1_MODULE);
        }
        group_enabled[group] = true;
    }
    
    return hal_timer_stop(timer_id);
}

// Set the counter to zero and configure the interval in microseconds.
esp_err_t hal_timer_configure(unsigned timer_id, uint32_t period_us)
{
    esp_err_t ret = hal_timer_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    unsigned timer = timer_id % 2U;
    if (period_us == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (TIMER(group, timer)->config & TIMER_EN) {
        return ESP_ERR_INVALID_STATE;
    }

    // APB / 80 = 1 MHz. Count up; keep the counter and alarm disabled during setup.
    TIMER(group, timer)->config = TIMER_DIV | TIMER_UP | TIMER_LEVEL_INT | TIMER_RELOAD;

    // Set both halves of the 64-bit counter to zero. LOAD applies the value.
    TIMER(group, timer)->loadlo = 0;
    TIMER(group, timer)->loadhi = 0;
    TIMER(group, timer)->load = 1;
    TIMER(group, timer)->alarmlo = period_us;
    TIMER(group, timer)->alarmhi = 0;
    return ESP_OK;
}

// Enable counting, the alarm, and the selected timer interrupt.
esp_err_t hal_timer_start(unsigned timer_id)
{
    esp_err_t ret = hal_timer_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }

    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    unsigned timer = timer_id % 2U;
    TIMG_INT_CLR(group) = TIMER_INT(timer);

    // INT_ENA is shared by both timers. Briefly mask local interrupts
    // to protect this read-modify-write operation. This does not lock the other CPU.
    unsigned saved = XTOS_SET_MIN_INTLEVEL(3);
    TIMG_INT_ENA(group) |= TIMER_INT(timer);
    XTOS_RESTORE_INTLEVEL(saved);
    TIMER(group, timer)->config |= TIMER_ALARM_EN | TIMER_EN;
    return ESP_OK;
}

// Stop the selected timer and clear its pending interrupt.
esp_err_t IRAM_ATTR hal_timer_stop(unsigned timer_id)
{
    esp_err_t ret = hal_timer_validate(timer_id);

    if (ret != ESP_OK) {
        return ret;
    }

    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    unsigned timer = timer_id % 2U;
    TIMER(group, timer)->config &= ~(TIMER_EN | TIMER_ALARM_EN);

    // Keep the other timer and watchdog enable bits unchanged.
    unsigned saved = XTOS_SET_MIN_INTLEVEL(3);
    TIMG_INT_ENA(group) &= ~TIMER_INT(timer);
    XTOS_RESTORE_INTLEVEL(saved);
    TIMG_INT_CLR(group) = TIMER_INT(timer);
    return ESP_OK;
}

// Clear the selected timer interrupt.
esp_err_t IRAM_ATTR hal_timer_ack(unsigned timer_id)
{
    esp_err_t ret = hal_timer_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }

    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    unsigned timer = timer_id % 2U;

    // Write one to clear only this bit. Do not use |= on a clear register.
    TIMG_INT_CLR(group) = TIMER_INT(timer);
    return ESP_OK;
}

// Enable the selected timer alarm again.
esp_err_t IRAM_ATTR hal_timer_rearm(unsigned timer_id)
{
    esp_err_t ret = hal_timer_validate(timer_id);
    if (ret != ESP_OK) {
        return ret;
    }

    // Only the HAL converts the ID into register coordinates.
    unsigned group = timer_id / 2U;
    unsigned timer = timer_id % 2U;

    // Auto-reload does not rearm ALARM_EN after an alarm.
    TIMER(group, timer)->config |= TIMER_ALARM_EN;
    return ESP_OK;
}