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

#define MYGPIO_PIN_COUNT              40
#define MYGPIO_FIRST_PIN_WITHOUT_PULLUP 34
#define MYGPIO_IOMUX_GPIO_FUNCTION    (2UL << MCU_SEL)
#define MYGPIO_IOMUX_FUNCTION_MASK    (7UL << MCU_SEL)
#define MYGPIO_IOMUX_PULLDOWN         (1UL << FUN_WPD)
#define MYGPIO_IOMUX_SLEEP_SELECT     (1UL << SLP_SEL)
#define MYGPIO_IOMUX_INPUT_ENABLE     (1UL << FUN_IE)
#define MYGPIO_IOMUX_PULLUP           (1UL << FUN_WPU)
#define MYGPIO_INTR_CPU0              0
#define MYGPIO_INTR_CPU0_ENABLE       4
#define MYGPIO_INTR_CPU1_ENABLE       1

typedef struct {
    intr_handler_t callback;
    void *ctx;
    intr_handle_t handle;
    volatile uint32_t *status;
    volatile uint32_t *clear;
    uint32_t mask;
} mygpio_intr_t;

static mygpio_intr_t gpio_interrupts[MYGPIO_PIN_COUNT];

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

/**
 * @brief Configure a GPIO input and register its edge interrupt callback.
 *
 * @param pin GPIO pin to configure.
 * @param type Rising, falling, or any-edge interrupt trigger.
 * @param cb Non-NULL callback invoked in interrupt context.
 * @param ctx User context passed to the callback.
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG for an invalid pin, trigger,
 *         or NULL callback, ESP_ERR_INVALID_STATE if the pin already has an
 *         interrupt registered, or an error returned by esp_intr_alloc().
 *
 * @todo Used espressif's SOC definitions because I have better stuff to do with my time
 */
esp_err_t mygpio_set_intr(uint8_t pin, mygpio_intr_type_t type,
                          intr_handler_t cb, void *ctx)
{
    if (pin >= MYGPIO_PIN_COUNT || IOMUX_ADDR[pin] == 0 || cb == NULL ||
        type < MYGPIO_INTR_RISING || type > MYGPIO_INTR_ANYEDGE) {
        return ESP_ERR_INVALID_ARG;
    }

    mygpio_intr_t *intr = &gpio_interrupts[pin];
    if (intr->handle != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    fill_interrupt(pin, intr);

    volatile uint32_t *iomux = (volatile uint32_t *)(uintptr_t)IOMUX_ADDR[pin];
    *iomux = (*iomux & ~(MYGPIO_IOMUX_FUNCTION_MASK | MYGPIO_IOMUX_PULLDOWN |
                        MYGPIO_IOMUX_SLEEP_SELECT)) |
             MYGPIO_IOMUX_GPIO_FUNCTION | MYGPIO_IOMUX_INPUT_ENABLE |
             (pin < MYGPIO_FIRST_PIN_WITHOUT_PULLUP ? MYGPIO_IOMUX_PULLUP : 0);

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
    GPIO.pin[pin].int_ena = esp_intr_get_cpu(intr->handle) == MYGPIO_INTR_CPU0
                               ? MYGPIO_INTR_CPU0_ENABLE
                               : MYGPIO_INTR_CPU1_ENABLE;
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
