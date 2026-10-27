/**
 * @file mygpio.c
 * @details Top level source file for mygpio library
 * @author Fernando R. Jacobo
 */
#include <stdint.h>
#include <stdio.h>
#include "mygpio.h"
#include "esp_err.h"
#include "esp_intr_types.h"
#include "esp_intr_alloc.h"
#include "soc/gpio_struct.h"
#include "soc/interrupts.h"
#include "regs.h"

#define SET_BIT(reg, x)   (reg |= (1UL << (x)))
#define UNSET_BIT(reg, x) (reg &= !(1UL << (x)))
#define GET_BIT(reg, x)   ((reg >> x) & 0x1)

typedef struct {
    intr_handler_t callback;
    void *ctx;
    intr_handle_t handle;
    volatile uint32_t *status;
    volatile uint32_t *clear;
    uint32_t mask;
} mygpio_intr_t;

static mygpio_intr_t gpio_interrupts[40];

static esp_err_t mygpio_config_iomux_in(volatile uint32_t *reg)
{
    UNSET_BIT(*reg, FUN_WPD);
    UNSET_BIT(*reg, SLP_SEL);
    SET_BIT(*reg, FUN_IE);
    SET_BIT(*reg, FUN_WPU);

    return ESP_OK;
}

static void mygpio_isr(void *arg)
{
    mygpio_intr_t *intr = arg;
    if (*intr->status & intr->mask) {
        *intr->clear = intr->mask;
        intr->callback(intr->ctx);
    }
}

/**
 * @brief fills mygpio_intr_t value
 *
 * @param pin gpio pin
 * @param intr [out] pinter to a mygpio_intr_t to fill
 */
static void fill_interrupt(uint8_t pin, mygpio_intr_t *intr)
{
    GPIO.pin[pin].int_ena = 0;
    intr->mask = 1UL << (pin & 31);

    if (pin < 32) {
        GPIO.enable_w1tc = intr->mask;
        intr->status = &GPIO.status;
        intr->clear = &GPIO.status_w1tc;
        return;
    }
    GPIO.enable1_w1tc.val = intr->mask;
    intr->status = &GPIO.status1.val;
    intr->clear = &GPIO.status1_w1tc.val;
}

esp_err_t mygpio_set_direction(uint8_t pin, mygpio_mode_t mode)
{
    if (IS_GPIO(pin) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mode == MYGPIO_MODE_OUTPUT) {
        SET_BIT(GPIO_ENABLE, pin);
        return ESP_OK;
    }

    UNSET_BIT(GPIO_ENABLE, pin); // Disable output mode

    if (mygpio_config_iomux_in((volatile uint32_t*)&IOMUX_ADDR[pin]) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mygpio_digital_write(uint8_t pin, bool state)
{
    if (IS_GPIO(pin) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    if (state) {
        SET_BIT(GPIO_ENABLE, pin);
        return ESP_OK;
    }

    UNSET_BIT(GPIO_ENABLE, pin);
    return ESP_OK;
}

esp_err_t mygpio_set_intr(uint8_t pin, mygpio_intr_type_t type,
                          intr_handler_t cb, void *ctx)
{
    if (pin >= 40 || IOMUX_ADDR[pin] == 0 || cb == NULL ||
        type < MYGPIO_INTR_RISING || type > MYGPIO_INTR_ANYEDGE) {
        return ESP_ERR_INVALID_ARG;
    }

    mygpio_intr_t *intr = &gpio_interrupts[pin];
    if (intr->handle != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    fill_interrupt(pin, intr);

    volatile uint32_t *iomux = (volatile uint32_t *)(uintptr_t)IOMUX_ADDR[pin];
    *iomux = (*iomux & ~((7UL << MCU_SEL) | (1UL << FUN_WPD) |
                        (1UL << SLP_SEL))) |
             (2UL << MCU_SEL) | (1UL << FUN_IE) |
             (pin < 34 ? (1UL << FUN_WPU) : 0);

    intr->callback = cb;
    intr->ctx = ctx;
    esp_err_t err = esp_intr_alloc(ETS_GPIO_INTR_SOURCE,
                                   ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LEVEL1,
                                   mygpio_isr, intr, &intr->handle);
    if (err != ESP_OK) {
        intr->callback = NULL;
        return err;
    }

    GPIO.pin[pin].int_type = type;
    *intr->clear = intr->mask;
    GPIO.pin[pin].int_ena = esp_intr_get_cpu(intr->handle) == 0 ? 4 : 1;
    return ESP_OK;
}

esp_err_t mygpio_get_level(uint8_t pin, bool *state)
{
    if (IS_GPIO(pin) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    *state = GET_BIT(GPIO_IN, pin);

    return ESP_OK;
}
