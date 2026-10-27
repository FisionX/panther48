/**
 * @file mygpio.c
 * @details Top level source file for mygpio library
 * @author Fernando R. Jacobo
 */
#include <stdio.h>
#include "mygpio.h"
#include "esp_err.h"
#include "esp_intr_types.h"
#include "regs.h"

#define SET_BIT(reg, x)   (reg |= (1UL << (x)))
#define UNSET_BIT(reg, x) (reg &= !(1UL << (x)))
#define GET_BIT(reg, x)   ((reg >> x) & 0x1)

static esp_err_t mygpio_config_iomux_in(volatile uint32_t *reg)
{
    UNSET_BIT(*reg, FUN_WPD);
    UNSET_BIT(*reg, SLP_SEL);
    SET_BIT(*reg, FUN_IE);
    SET_BIT(*reg, FUN_WPU);

    return ESP_OK;
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

esp_err_t mygpio_set_intr(uint8_t pin, intr_handler_t cb)
{
    if (IS_GPIO(pin) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    // TODO: set gpio direction in and setup callback

    return ESP_ERR_NOT_FINISHED;
}

esp_err_t mygpio_get_level(uint8_t pin, bool *state)
{
    if (IS_GPIO(pin) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    *state = GET_BIT(GPIO_IN, pin);

    return ESP_OK;
}
