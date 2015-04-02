/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gpio_api.h"
#include "pinmap.h"

/* PORTING STEP 3a: Implement "gpio_init" */

uint32_t gpio_set(PinName pin) {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
#ifdef TARGET_LPC2368
    LPC_SC->SCS |= 1; // High speed GPIO is enabled on ports 0 and 1
#endif
    pin_function(pin, 0);

#elif defined(TARGET_LPC11U24)
    int f = ((pin == P0_11) ||
             (pin == P0_12) ||
             (pin == P0_13) ||
             (pin == P0_14)) ? (1) : (0);
    pin_function(pin, f);
#elif defined(TARGET_LPC11Cxx)
    // TODO(bracz) check this
    pin_function(pin, 0);
#else
#error CPU undefined.
#endif
    return (1 << ((int)pin & 0x1F));
}

void gpio_init(gpio_t *obj, PinName pin, PinDirection direction) {
    if(pin == NC) return;

    obj->pin = pin;
    obj->mask = gpio_set(pin);

#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_GPIO_TypeDef *port_reg = (LPC_GPIO_TypeDef *) ((int)pin & ~0x1F);

    obj->reg_set = &port_reg->FIOSET;
    obj->reg_clr = &port_reg->FIOCLR;
    obj->reg_in  = &port_reg->FIOPIN;
    obj->reg_dir = &port_reg->FIODIR;

#elif defined(TARGET_LPC11U24)
    unsigned int port = (unsigned int)pin >> PORT_SHIFT;

    obj->reg_set = &LPC_GPIO->SET[port];
    obj->reg_clr = &LPC_GPIO->CLR[port];
    obj->reg_in  = &LPC_GPIO->PIN[port];
    obj->reg_dir = &LPC_GPIO->DIR[port];
#elif  defined(TARGET_LPC11Cxx)
    int gpionum = ((int) pin) >> 5;
    int pinnum = ((int) pin) & 0x1f;
    LPC_GPIO_TypeDef* lpc = 0;
    switch(gpionum) {
      case 0: lpc = LPC_GPIO0; break;
      case 1: lpc = LPC_GPIO1; break;
      case 2: lpc = LPC_GPIO2; break;
      case 3: lpc = LPC_GPIO3; break;
      default: {
        extern void abort(void); abort();
      }
    }
    obj->reg_dir = &lpc->DIR;
    obj->reg_set = &lpc->MASKED_ACCESS[1<<pinnum];
    // TODO(bracz) this is not strictly speaking true. You have to write 0 here
    // to clear the pin state.
    obj->reg_clr = &lpc->MASKED_ACCESS[1<<pinnum];
    obj->reg_in = &lpc->DATA;
#else
#error CPU undefined.
#endif

    gpio_dir(obj, direction);
    switch (direction) {
        case PIN_OUTPUT: pin_mode(pin, PullNone); break;
        case PIN_INPUT : pin_mode(pin, PullDown); break;
    }
}

void gpio_mode(gpio_t *obj, PinMode mode) {
    pin_mode(obj->pin, mode);
}

void gpio_dir(gpio_t *obj, PinDirection direction) {
    switch (direction) {
        case PIN_INPUT : *obj->reg_dir &= ~obj->mask; break;
        case PIN_OUTPUT: *obj->reg_dir |=  obj->mask; break;
    }
}
