/* mbed Microcontroller Library - cmsis_nvic
 * Copyright (c) 2009-2011 ARM Limited. All rights reserved.
 *
 * CMSIS-style functionality to support dynamic vectors
 */ 

#ifndef MBED_CMSIS_NVIC_H
#define MBED_CMSIS_NVIC_H

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INTERRUPT_ATTRIBUTE
#define INTERRUPT_ATTRIBUTE __attribute__((__interrupt__("IRQ")))
#define INTERRUPT_ACK 1
#endif

void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector);
uint32_t NVIC_GetVector(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif

#endif
