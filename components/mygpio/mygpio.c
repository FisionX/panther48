/**
 * @file mygpio.c
 * @details Top level source file for mygpio library
 * @author Fernando R. Jacobo
 */
#include <stdio.h>
#include "mygpio.h"
#include "esp_err.h"
#include "mygpio_hal.h"

#define IS_IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

#define IS_GPIO(val) ((             \
    (val) > 19)                     \
    || IS_IN_RANGE((val), 21, 23)   \
    || IS_IN_RANGE((val), 25, 27)   \
    || IS_IN_RANGE((val), 32, 39)   \
)

#define SET_BIT(reg, x)   (reg |= (1UL << (uint8_t)x))
#define UNSET_BIT(reg, x) (reg &= !(1UL << (uint8_t)x))
#define GET_BIT(reg, x)   ((reg >> x) & 0x1)

static const uint32_t IOMUX_ADDR[] =
{
    [0]  = 0x3FF49044,
    [1]  = 0x3FF49088,
    [2]  = 0x3FF49040,
    [3]  = 0x3FF49084,
    [4]  = 0x3FF49048,
    [5]  = 0x3FF4906C,
    [6]  = 0x3FF49060,
    [7]  = 0x3FF49064,
    [8]  = 0x3FF49068,
    [9]  = 0x3FF49054,
    [10] = 0x3FF49058,
    [11] = 0x3FF4905C,
    [12] = 0x3FF49034,
    [13] = 0x3FF49038,
    [14] = 0x3FF49030,
    [15] = 0x3FF4903C,
    [16] = 0x3FF4904C,
    [17] = 0x3FF49050,
    [18] = 0x3FF49070,
    [19] = 0x3FF49074,

    [21] = 0x3FF4907C,
    [22] = 0x3FF49080,
    [23] = 0x3FF4908C,

    [25] = 0x3FF49024,
    [26] = 0x3FF49028,
    [27] = 0x3FF4902C,

    [32] = 0x3FF4901C,
    [33] = 0x3FF49020,
    [34] = 0x3FF49014,
    [35] = 0x3FF49018,
    [36] = 0x3FF49004,
    [37] = 0x3FF49008,
    [38] = 0x3FF4900C,
    [39] = 0x3FF49010
};

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
    if (IS_GPIO(pin)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mode == MYGPIO_MODE_OUTPUT) {
        SET_BIT(GPIO_ENABLE, pin);
        return ESP_OK;
    }

    UNSET_BIT(GPIO_ENABLE, pin); // Disable output mode

    if (mygpio_config_iomux_in((volatile uint32_t*)IOMUX_ADDR[pin]) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mygpio_digital_write(uint8_t pin, bool state)
{
    if (IS_GPIO(pin)) {
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
    if (IS_GPIO(pin)) {
        return ESP_ERR_INVALID_ARG;
    }

    *state = GET_BIT(GPIO_IN, pin);

    return ESP_OK;
}
