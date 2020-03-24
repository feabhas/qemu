/*
 * STM32F40x System-on-Chip general purpose input/output register definition
 *
 * Reference Manual: http://infocenter.nordicsemi.com/pdf/nRF51_RM_v3.0.pdf
 * Product Spec: http://infocenter.nordicsemi.com/pdf/nRF51822_PS_v3.1.pdf
 *
 * Copyright 2018 Steffen Görtz <contrib@steffen-goertz.de>
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

/*
 * Check if the output driver is connected to the direction switch
 * given the current configuration and logic level.
 * It is not differentiated between standard and "high"(-power) drive modes.
 */
// static bool is_connected(uint32_t config, uint32_t level)
// {
//     bool state;
//     uint32_t drive_config = extract32(config, 8, 3);

//     switch (drive_config)
//     {
//     case 0 ... 3:
//         state = true;
//         break;
//     case 4 ... 5:
//         state = level != 0;
//         break;
//     case 6 ... 7:
//         state = level == 0;
//         break;
//     default:
//         g_assert_not_reached();
//         break;
//     }

//     return state;
// }

// static int pull_value(uint32_t config)
// {
//     int pull = extract32(config, 2, 2);
//     if (pull == STM32F40x_GPIO_PULLDOWN)
//     {
//         return 0;
//     }
//     else if (pull == STM32F40x_GPIO_PULLUP)
//     {
//         return 1;
//     }
//     return -1;
// }

static void update_output_irq(STM32F40xGPIOState *s, size_t i,
                              bool connected, bool level)
{
    // int64_t irq_level = connected ? level : -1;
    // bool old_connected = extract32(s->old_out_connected, i, 1);
    // bool old_level = extract32(s->old_out, i, 1);

    // if ((old_connected != connected) || (old_level != level))
    // {
    //     qemu_set_irq(s->output[i], irq_level);
    //     // trace_stm32f40x_gpio_update_output_irq(i, irq_level);
    // }

    // s->old_out = deposit32(s->old_out, i, 1, level);
    // s->old_out_connected = deposit32(s->old_out_connected, i, 1, connected);
}

static void update_state(STM32F40xGPIOState *s)
{
    // int pull;
    // size_t i;
    // bool connected_out, dir, connected_in, out, in, input;

    // for (i = 0; i < STM32F40x_GPIO_PINS; i++)
    // {
    //     pull = pull_value(s->cnf[i]);
    //     dir = extract32(s->cnf[i], 0, 1);
    //     connected_in = extract32(s->in_mask, i, 1);
    //     out = extract32(s->out, i, 1);
    //     in = extract32(s->in, i, 1);
    //     input = !extract32(s->cnf[i], 1, 1);
    //     connected_out = is_connected(s->cnf[i], out) && dir;

    //     if (!input)
    //     {
    //         if (pull >= 0)
    //         {
    //             /* Input buffer disconnected from external drives */
    //             s->in = deposit32(s->in, i, 1, pull);
    //         }
    //     }
    //     else
    //     {
    //         if (connected_out && connected_in && out != in)
    //         {
    //             /* Pin both driven externally and internally */
    //             qemu_log_mask(LOG_GUEST_ERROR,
    //                           "GPIO pin %zu short circuited\n", i);
    //         }
    //         if (!connected_in)
    //         {
    //             /*
    //              * Floating input: the output stimulates IN if connected,
    //              * otherwise pull-up/pull-down resistors put a value on both
    //              * IN and OUT.
    //              */
    //             if (pull >= 0 && !connected_out)
    //             {
    //                 connected_out = true;
    //                 out = pull;
    //             }
    //             if (connected_out)
    //             {
    //                 s->in = deposit32(s->in, i, 1, out);
    //             }
    //         }
    //     }
    //     update_output_irq(s, i, connected_out, out);
    // }
}

/*
 * Direction is exposed in both the DIR register and the DIR bit
 * of each PINs CNF configuration register. Reflect bits for pins in DIR
 * to individual pin configuration registers.
 */
// static void reflect_dir_bit_in_cnf(STM32F40xGPIOState *s)
// {
//     size_t i;

//     uint32_t value = s->dir;

//     for (i = 0; i < STM32F40x_GPIO_PINS; i++)
//     {
//         s->cnf[i] = (s->cnf[i] & ~(1UL)) | ((value >> i) & 0x01);
//     }
// }

static uint64_t stm32f40x_gpio_read(void *opaque, hwaddr offset, unsigned int size)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(opaque);
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
        r = s->BSRR;
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

    trace_stm32f40x_gpio_read(offset, r);

    return r;
}

static void stm32f40x_gpio_write(void *opaque, hwaddr offset,
                                 uint64_t value, unsigned int size)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(opaque);

    trace_stm32f40x_gpio_write(offset, value);

    switch (offset)
    {
    case STM32F40x_GPIO_REG_MODER:
        s->MODER = value;
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
        s->IDR = value;
        break;
    case STM32F40x_GPIO_REG_ODR:
        s->ODR = value;
        break;
    case STM32F40x_GPIO_REG_BSRR:
        s->BSRR = value;
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

    update_state(s);
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

    trace_stm32f40x_gpio_set(line, value);

    assert(line >= 0 && line < STM32F40x_GPIO_PINS);

    // s->in_mask = deposit32(s->in_mask, line, 1, value >= 0);
    // if (value >= 0)
    // {
    //     s->in = deposit32(s->in, line, 1, value != 0);
    // }

    update_state(s);
}

static void stm32f40x_gpio_reset(DeviceState *dev)
{
    STM32F40xGPIOState *s = STM32F40x_GPIO(dev);
    // size_t i;

    // s->out = 0;
    // s->old_out = 0;
    // s->old_out_connected = 0;
    // s->in = 0;
    // s->in_mask = 0;
    // s->dir = 0;

    // for (i = 0; i < STM32F40x_GPIO_PINS; i++)
    // {
    //     s->cnf[i] = 0x00000002;
    // }
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
