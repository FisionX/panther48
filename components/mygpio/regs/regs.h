/**
 * @file mygpio_hal.h
 * @details Hardware Abstraction definitions for mygpio library
 * This library is just a demonstration, nobody with a perfectly
 * working brain would directly mess with the plain registers on
 * a real application...
 * @author Fernando R. Jacobo
 */
#pragma once
#ifndef MY_GPIO_HAL_H
#define MY_GPIO_HAL_H

#include <stdio.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS_IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

#define IS_GPIO(val) (              \
    ((val) > 19)                    \
    || IS_IN_RANGE((val), 21, 23)   \
    || IS_IN_RANGE((val), 25, 27)   \
    || IS_IN_RANGE((val), 32, 39)   \
)

#define HWREG32(x)		(*((volatile uint32_t *)(x)))
#define BIT_MSK(X)      (1UL<<X)

//GPIO
#if CONFIG_IDF_TARGET_ESP32

#define GPIO_OUT			(HWREG32(0x3FF44004))
#define GPIO_W1TS			(HWREG32(0x3FF44008))
#define GPIO_W1TC			(HWREG32(0x3FF4400C))
#define GPIO_ENABLE			(HWREG32(0x3FF44020))
#define GPIO_ENABLE_W1TS	(HWREG32(0x3FF44024))
#define GPIO_ENABLE_W1TC	(HWREG32(0x3FF44028))
#define GPIO_IN				(HWREG32(0x3FF4403C))

#define GPIO_PIN(n)       (HWREG32(0x3FF44088 + (x04 * (n))))

#define FUN_IE  9
#define MCU_SEL 12
#define FUN_WPU 8
#define FUN_WPD 7
#define SLP_SEL 1

#endif // CONFIG_IDF_TARGET_ESP32

extern const uint32_t IOMUX_ADDR[40];

#ifdef __cplusplus
}
#endif

#endif // MY_GPIO_HAL_H
