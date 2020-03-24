/*
 * STM32F40x System-on-Chip general purpose input/output register definition
 *
 * QEMU interface:
 * + sysbus MMIO regions 0: GPIO registers
 * + Unnamed GPIO inputs 0-31: Set tri-state input level for GPIO pin.
 *   Level -1: Externally Disconnected/Floating; Pull-up/down will be regarded
 *   Level 0: Input externally driven LOW
 *   Level 1: Input externally driven HIGH
 * + Unnamed GPIO outputs 0-31:
 *   Level -1: Disconnected/Floating
 *   Level 0: Driven LOW
 *   Level 1: Driven HIGH
 *
 * Accuracy of the peripheral model:
 * + The STM32F40x GPIO output driver supports two modes, standard and
 * high-current mode. These different drive modes are not modeled and handled
 * the same.
 * + Pin SENSEing is not modeled/implemented.
 *
 * Copyright 2018 Steffen Görtz <contrib@steffen-goertz.de>
 *
 * This code is licensed under the GPL version 2 or later.  See
 * the COPYING file in the top-level directory.
 *
 */
#ifndef STM32F40x_GPIO_H
#define STM32F40x_GPIO_H

#include "hw/sysbus.h"
#define TYPE_STM32F40x_GPIO "stm32f40x_soc.gpio"
#define STM32F40x_GPIO(obj)                                                    \
  OBJECT_CHECK(STM32F40xGPIOState, (obj), TYPE_STM32F40x_GPIO)

#define STM32F40x_GPIO_PINS 16

#define STM32F40x_GPIO_SIZE 0x1000

#define STM32F40x_GPIO_REG_MODER 0x00
#define STM32F40x_GPIO_REG_OTYPE 0x04
#define STM32F40x_GPIO_REG_OSPEEDR 0x08
#define STM32F40x_GPIO_REG_PUPDR 0x0C
#define STM32F40x_GPIO_REG_IDR 0x10
#define STM32F40x_GPIO_REG_ODR 0x14
#define STM32F40x_GPIO_REG_BSRR 0x18
#define STM32F40x_GPIO_REG_LCKR 0x1C
#define STM32F40x_GPIO_REG_AFRL 0x20
#define STM32F40x_GPIO_REG_AFRH 0x24

#define STM32F40x_GPIO_PULLDOWN 1
#define STM32F40x_GPIO_PULLUP 3

typedef struct STM32F40xGPIOState {
  SysBusDevice parent_obj;

  MemoryRegion mmio;

  qemu_irq irq[STM32F40x_GPIO_PINS];
  qemu_irq output[STM32F40x_GPIO_PINS];

  //   uint32_t out;
  //   uint32_t in;
  //   uint32_t in_mask;
  //   uint32_t dir;
  //   uint32_t cnf[STM32F40x_GPIO_PINS];

  //   uint32_t old_out;
  //   uint32_t old_out_connected;

  uint32_t MODER;
  uint32_t OTYPER;
  uint32_t OSPEEDR;
  uint32_t PUPDR;
  uint32_t IDR;
  uint32_t ODR;
  uint32_t BSRR;
  uint32_t LCKR;
  uint32_t AFR[2];

} STM32F40xGPIOState;

#endif
