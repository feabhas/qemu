/*
 * STM32F40x System-on-Chip general purpose input/output register definition
 *
 * Copyright 2020 Niall Cooling <nsc@acm.org>
 *
 * This code is licensed under the GPL version 2 or later.  See
 * the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/gpio/stm32f40x_gpio.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "trace.h"

static uint64_t stm32f40x_gpio_read(void *opaque, hwaddr offset, unsigned int size)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(opaque);
    uint64_t base_addr = s->mmio.addr;

    uint64_t r = 0;

    switch (offset)
    {
    case STM32F40x_GPIO_REG_MODER:
        r = s->MODER;
        break;
    case STM32F40x_GPIO_REG_OTYPE:
        r = s->OTYPER;
        break;
    case STM32F40x_GPIO_REG_OSPEEDR:
        r = s->OSPEEDR;
        break;
    case STM32F40x_GPIO_REG_PUPDR:
        r = s->PUPDR;
        break;
    case STM32F40x_GPIO_REG_IDR:
        r = s->IDR;
        break;
    case STM32F40x_GPIO_REG_ODR:
        r = s->ODR;
        break;
    case STM32F40x_GPIO_REG_BSRR:
        // Error read-only register
        // returns 0
        break;
    case STM32F40x_GPIO_REG_LCKR:
        r = s->LCKR;
        break;
    case STM32F40x_GPIO_REG_AFRL:
        r = s->AFR[0];
        break;
    case STM32F40x_GPIO_REG_AFRH:
        r = s->AFR[1];
        break;

    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: bad read offset 0x%" HWADDR_PRIx "\n",
                      __func__, offset);
    }

    trace_stm32f40x_gpio_read(base_addr + offset, r);

    return r;
}

enum MODES
{
    Input,
    Output,
    AF,
    Analog
};

// helper function to setup I/O masks
static void set_masks(STM32F40xGPIOState *s)
{
    uint32_t moder = s->MODER;
    for (unsigned i = 0; i < 16; ++i)
    {
        uint32_t mode = moder & 0x3; // two bits for mode
        switch (mode)
        {
        case Output:
            s->out_mask |= (1 << i);
            // fallthrough as outputs read as inputs
        case Input:
            s->in_mask |= (1 << i);
            break;
        default:
            // AF and Analog not yet supported
            s->in_mask &= ~(1 << i);
            s->out_mask &= ~(1 << i);
        }
        moder = moder >> 2; // mode of next gpio pin
    }
}

static void ODR_write(STM32F40xGPIOState *s, uint32_t value)
{
    s->ODR &= ~(s->out_mask); // clear all output bits
    s->ODR |= (value & s->out_mask);
    s->IDR &= ~(s->out_mask); // clear all output bits
    s->IDR |= (value & s->out_mask);
}

static void BRR_write(STM32F40xGPIOState *s, uint32_t value)
{
    uint32_t set_values = value & 0xFFFF;
    uint32_t reset_values = (value >> 16) & 0xFFFF;

    s->ODR &= ~(reset_values & s->out_mask);
    s->ODR |= (set_values & s->out_mask);
    s->IDR &= ~(reset_values & s->out_mask);
    s->IDR |= (set_values & s->out_mask);
}

static void stm32f40x_gpio_write(void *opaque, hwaddr offset,
                                 uint64_t value, unsigned int size)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(opaque);
    uint64_t base_addr = s->mmio.addr;
    trace_stm32f40x_gpio_write(base_addr + offset, value);

    // TODO
    // need to check GPIO device is enabled in RCC

    switch (offset)
    {
    case STM32F40x_GPIO_REG_MODER:
        s->MODER = value;
        set_masks(s);
        break;
    case STM32F40x_GPIO_REG_OTYPE:
        s->OTYPER = value;
        break;
    case STM32F40x_GPIO_REG_OSPEEDR:
        s->OSPEEDR = value;
        break;
    case STM32F40x_GPIO_REG_PUPDR:
        s->PUPDR = value;
        break;
    case STM32F40x_GPIO_REG_IDR:
        // s->IDR = value;
        // error write-only register
        break;
    case STM32F40x_GPIO_REG_ODR:
        // s->ODR = value;
        ODR_write(s, value);
        break;
    case STM32F40x_GPIO_REG_BSRR:
        // s->BSRR = value;
        BRR_write(s, value);
        break;
    case STM32F40x_GPIO_REG_LCKR:
        s->LCKR = value;
        break;
    case STM32F40x_GPIO_REG_AFRL:
        s->AFR[0] = value;
        break;
    case STM32F40x_GPIO_REG_AFRH:
        s->AFR[1] = value;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: bad write offset 0x%" HWADDR_PRIx "\n",
                      __func__, offset);
    }
}

static const MemoryRegionOps gpio_ops = {
    .read = stm32f40x_gpio_read,
    .write = stm32f40x_gpio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl.min_access_size = 4,
    .impl.max_access_size = 4,
};

static void stm32f40x_gpio_set(void *opaque, int line, int value)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(opaque);
    // uint64_t base_addr = s->mmio.addr;

    trace_stm32f40x_gpio_set(line, value);

    assert(line >= 0 && line < STM32F40x_GPIO_PINS);

    // s->in_mask = deposit32(s->in_mask, line, 1, value >= 0);
    // if (value >= 0)
    // {
    //     s->in = deposit32(s->in, line, 1, value != 0);
    // }

    // update_state(s);
}

static void stm32f40x_gpio_reset(DeviceState *dev)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(dev);

    uint64_t base_addr = s->mmio.addr;

    trace_stm32f40x_gpio_reset(s->mmio.name, base_addr);

    // everything defaults to input
    s->MODER = 0;
    s->OTYPER = 0;
    s->OSPEEDR = 0;
    s->PUPDR = 0;
    s->IDR = 0;
    s->ODR = 0;
    s->BSRR = 0;
    s->LCKR = 0;
    s->AFR[0] = 0;
    s->AFR[1] = 0;

    if (base_addr == 0x40020000)
    { // GPIOA
        s->MODER = 0xA8000000;
        // s->PUPDR =
    }
    if (base_addr == 0x40020400)
    { // GPIOB
        s->MODER = 0x00000280;
        // s->OSPEEDR =
        // s->PUPDR =
    }
    set_masks(s);
}

static const VMStateDescription vmstate_stm32f40x_gpio = {
    .name = TYPE_STM32F40x_GPIO,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]){
        VMSTATE_UINT32(MODER, STM32F40xGPIOState),
        VMSTATE_UINT32(OTYPER, STM32F40xGPIOState),
        VMSTATE_UINT32(OSPEEDR, STM32F40xGPIOState),
        VMSTATE_UINT32(PUPDR, STM32F40xGPIOState),
        VMSTATE_UINT32(IDR, STM32F40xGPIOState),
        VMSTATE_UINT32(ODR, STM32F40xGPIOState),
        VMSTATE_UINT32(BSRR, STM32F40xGPIOState),
        VMSTATE_UINT32(LCKR, STM32F40xGPIOState),
        VMSTATE_UINT32_ARRAY(AFR, STM32F40xGPIOState, 2),
        VMSTATE_END_OF_LIST()}};

static void stm32f40x_gpio_init(Object *obj)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(obj);

    memory_region_init_io(&s->mmio, obj, &gpio_ops, s,
                          TYPE_STM32F40x_GPIO, STM32F40x_GPIO_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    qdev_init_gpio_in(DEVICE(s), stm32f40x_gpio_set, STM32F40x_GPIO_PINS);
    qdev_init_gpio_out(DEVICE(s), s->output, STM32F40x_GPIO_PINS);
}

static void stm32f40x_gpio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &vmstate_stm32f40x_gpio;
    dc->reset = stm32f40x_gpio_reset;
    dc->desc = "STM32F40x GPIO";
}

static const TypeInfo stm32f40x_gpio_info = {
    .name = TYPE_STM32F40x_GPIO,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32F40xGPIOState),
    .instance_init = stm32f40x_gpio_init,
    .class_init = stm32f40x_gpio_class_init};

static void stm32f40x_gpio_register_types(void)
{
    type_register_static(&stm32f40x_gpio_info);
}

type_init(stm32f40x_gpio_register_types)
