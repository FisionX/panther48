/**
 * @file mygpio.h
 * @details Public API for mygpio demo library
 * This library is just a demonstration, nobody with a perfectly
 * working brain would directly mess with the plain registers on
 * a real application...
 * @author Fernando R. Jacobo
 */
#pragma once
#ifndef MY_GPIO_H
#define MY_GPIO_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GPIO_MODE {
    MYGPIO_MODE_OUTPUT,
    MYGPIO_MODE_INPUT
} mygpio_mode_t;

esp_err_t mygpio_set_direction(uint8_t pin, mygpio_mode_t mode);
esp_err_t mygpio_digital_write(uint8_t pin, bool state);
esp_err_t mygpio_get_level(uint8_t pin, bool *state);

#ifdef __cplusplus
}
#endif

#endif // MY_GPIO_H
