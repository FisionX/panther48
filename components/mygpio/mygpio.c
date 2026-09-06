/**
 * @file mygpio.c
 * @details Top level source file for mygpio library
 * @author Fernando R. Jacobo
 */
#include <stdio.h>
#include "mygpio.h"
#include "esp_err.h"
#include "mygpio_hal.h"

#define SET_BIT(reg,x)   (reg |= (1UL << (uint8_t)x))
#define UNSET_BIT(reg,x) (reg &= !(1UL << (uint8_t)x))
#define GET_BIT(reg,x)   ((reg >> x) & 0x1)

esp_err_t mygpio_set_direction(uint8_t pin, mygpio_mode_t mode)
{
    if (pin > GPIO_PIN_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mode == MYGPIO_MODE_OUTPUT) {
        SET_BIT(GPIO_ENABLE, pin);
        return ESP_OK;
    }

    UNSET_BIT(GPIO_ENABLE, pin);

    return ESP_OK;
}

esp_err_t mygpio_digital_write(uint8_t pin, bool state)
{
    if (pin > GPIO_PIN_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (state) {
        SET_BIT(GPIO_ENABLE, pin);
        return ESP_OK;
    }

    UNSET_BIT(GPIO_ENABLE, pin);
    return ESP_OK;
}

esp_err_t mygpio_get_level(uint8_t pin, bool *state)
{
    if (pin > GPIO_PIN_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    *state = GET_BIT(GPIO_IN, pin);

    return ESP_OK;
}

esp_err_t mygpio_config_iomux18(void)
{
    UNSET_BIT(IOMUX_18, FUN_WPD);
    UNSET_BIT(IOMUX_18, SLP_SEL);
    SET_BIT(IOMUX_18, FUN_IE);
    SET_BIT(IOMUX_18, FUN_WPU);

    return ESP_OK;
}
