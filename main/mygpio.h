#pragma once
#ifndef MY_GPIO_H
#define MY_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

//Macro principal
#define HWREG32(x)			(*((volatile uint32_t *)(x)))

//GPIO
#define GPIO_OUT			(HWREG32(0x3FF44004))
#define GPIO_W1TS			(HWREG32(0x3FF44008))
#define GPIO_W1TC			(HWREG32(0x3FF4400C))
#define GPIO_ENABLE			(HWREG32(0x3FF44020))
#define GPIO_ENABLE_W1TS	(HWREG32(0x3FF44024))
#define GPIO_ENABLE_W1TC	(HWREG32(0x3FF44028))
#define GPIO_IN				(HWREG32(0x3FF4403C))
//IOMUX
#define IOMUX_18			(HWREG32(0x3FF44070))
#define IOMUX_19			(HWREG32(0x3FF44074))
#define IOMUX_12			(HWREG32(0x3FF49034))
#define IOMUX_13			(HWREG32(0x3FF49038))
#define IOMUX_14			(HWREG32(0x3FF49030))

#define FUN_IE (1UL<<9)
#define MCU_SEL (1UL<<13)
#define FUN_WPU (1UL<<8)
#define _BIT(X) (1UL<<X)
#define BTN1 18
#define BTN2 19

#ifdef __cplusplus
}
#endif
#endif // MY_GPIO_H
