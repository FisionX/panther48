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

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HWREG32(x)		(*((volatile uint32_t *)(x)))
#define BIT_MSK(X)      (1UL<<X)

//GPIO
#if CONFIG_IDF_TARGET_ESP32

#define GPIO_PIN_COUNT 32

#define GPIO_OUT			(HWREG32(0x3FF44004))
#define GPIO_W1TS			(HWREG32(0x3FF44008))
#define GPIO_W1TC			(HWREG32(0x3FF4400C))
#define GPIO_ENABLE			(HWREG32(0x3FF44020))
#define GPIO_ENABLE_W1TS	(HWREG32(0x3FF44024))
#define GPIO_ENABLE_W1TC	(HWREG32(0x3FF44028))
#define GPIO_IN				(HWREG32(0x3FF4403C))

//IOMUX

#define IOMUX_18			(HWREG32(0x3FF49070))
#define IOMUX_19			(HWREG32(0x3FF49074))
#define IOMUX_12			(HWREG32(0x3FF49034))
#define IOMUX_13			(HWREG32(0x3FF49038))
#define IOMUX_14			(HWREG32(0x3FF49030))
#define IOMUX_04			(HWREG32(0x3FF49048))

#endif // CONFIG_IDF_TARGET_ESP32

#define FUN_IE  9
#define MCU_SEL 12
#define FUN_WPU 8
#define FUN_WPD 7
#define SLP_SEL 1

#ifdef __cplusplus
}
#endif

#endif // MY_GPIO_HAL_H
