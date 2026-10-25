/** 
 * @file timer_regs.h
 *
 * @details Private Mapping of ESP32 timer registries.
 *
 * @author Alejandro Rocha R.
 */

#pragma once
#ifndef TIMER_REGS_H
#define TIMER_REGS_H

#include <stdint.h>

#define HWREG32(x) (*(volatile uint32_t *)(uintptr_t)(x)) // 32-bit register address mask

// 9 * 32-bit registers per timer (0x24 bytes).
typedef struct {
    uint32_t config;  // +0x00: Enabling, Divider, Direction, Timer mode
    uint32_t lo;      // +0x04: Low 32-bit counter value.   
    uint32_t hi;      // +0x08: High 32-bit counter value.    
    uint32_t update;  // +0x0C: Request counter (lo/hi)    
    uint32_t alarmlo; // +0x10: Low 32-bit alarm value.
    uint32_t alarmhi; // +0x14: High 32-bit alarm value.
    uint32_t loadlo;  // +0x18: Low 32-bit reload value.
    uint32_t loadhi;  // +0x1C: High 32-bit reload value.
    uint32_t load;    // +0x20: Apply reload
} timer_regs_t;

#define TIMG_REG 0x3FF5F000UL //Timer group base address

// Validar group/timer antes de usar estas macros: no hacen guardas.

#define TIMG_BASE(group) (TIMG_REG + 0x1000UL * (group)) // TG0 -> TG1 Shift (0x24 bytes per timer)
#define TIMER(group, timer) ((volatile timer_regs_t *)(uintptr_t)(TIMG_BASE(group) + 0x24UL * (timer)))

#define TIMG_INT_ENA(group) HWREG32(TIMG_BASE(group) + 0x098UL)
#define TIMG_INT_CLR(group) HWREG32(TIMG_BASE(group) + 0x0A4UL)
#define TIMER_INT(timer) (1UL << (timer))


// Simplify to  select 1 - 4?????


// Bits del registro config: conteo, direccion, recarga, divisor y alarma.
#define TIMER_EN        (1UL << 31)
#define TIMER_UP        (1UL << 30)
#define TIMER_RELOAD    (1UL << 29)

// Divisor almacenado en bits [28:13]; APB fijo de 80 MHz / 80 = 1 MHz.
#define TIMER_DIV       (80UL << 13)
#define TIMER_LEVEL_INT (1UL << 11)
#define TIMER_ALARM_EN  (1UL << 10)

#endif