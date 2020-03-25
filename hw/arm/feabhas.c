/*
 * Feabhas Training Board Machine Model
 *
 * Copyright (c) 2020 Niall Cooling <nsc@acm.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "qemu/error-report.h"
#include "hw/arm/stm32f407_soc.h"
#include "hw/arm/boot.h"
#include "hw/irq.h"

#define NUM_IRQ_LINES 64
#define NUM_OF_GPIOS 4

static const uint32_t gpio_addr[NUM_OF_GPIOS] =
    {0x40020000, 0x40020400, 0x40020800, 0x40020c00};

enum GPIOS
{
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH,
    GPIOI
};

static void gpio_port_handler(void *opaque, int n, int level)
{
    // Stm32FeabhasWms *s = (Stm32FeabhasWms *)opaque;

    printf("n:%d l:%d\n", n, level);

    // static unsigned ss_value = 0;
    // switch (n)
    // {
    // case 0:
    // case 1:
    // case 2:
    // case 3:
    // {
    //     if (level == 0)
    //     {
    //         s->ss_value &= ~(1 << n);
    //     }
    //     else
    //     {
    //         s->ss_value |= (1 << n);
    //     }
    //     printf("7-Segment: %u\n", s->ss_value);
    // }
    // break;
    // case 5:
    //     printf("Motor is %s\n", !level ? "OFF" : "ON");
    //     break;
    // case 6:
    //     printf("Motor is %s\n", !level ? "CLK" : "ACLK");
    //     break;
    // case 7:
    //     printf("Latch is %s\n", !level ? "LOW" : "HIGH");
    //     s->latch = (bool)level;
    //     break;
    // case 8:
    //     printf("Buzzer is %s\n", !level ? "OFF" : "ON");
    //     break;
    // default:
    //     break;
    // }
}

static void
feabhas_init(MachineState *machine)
{
    DeviceState *dev = qdev_create(NULL, TYPE_STM32F407_SOC);

    // DeviceState *dev = DEVICE(obj);
    STM32F407State *s = STM32F407_SOC(dev);
    // SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
    object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);

    // MemoryRegion *mr = sysbus_mmio_get_region(SYS_BUS_DEVICE("gpio[d]"), 0);

    // DeviceState *gpio_d = DEVICE(object_resolve_path("/machine/stm32f407/gpio[d]", NULL));
    // assert(gpio_d);

    STM32F40xGPIOState *gpio_d = &s->gpio[GPIOD];

    // // Connect LED to GPIO D pin 8,9,10 & 11
    qemu_irq *led_irq = qemu_allocate_irqs(gpio_port_handler, NULL, 4);
    // sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));

    // qdev_connect_gpio_out(DEVICE(&s->adc_irqs), 0,
    //                       qdev_get_gpio_in(armv7m, ADC_IRQ));
    qdev_connect_gpio_out(gpio_d, 8, led_irq[0]);
    qdev_connect_gpio_out(gpio_d, 9, led_irq[1]);
    qdev_connect_gpio_out(gpio_d, 10, led_irq[2]);
    qdev_connect_gpio_out(gpio_d, 11, led_irq[3]);

    // DeviceState *nvic;

    // nvic = qdev_create(NULL, TYPE_ARMV7M);
    // qdev_prop_set_uint32(nvic, "num-irq", NUM_IRQ_LINES);
    // qdev_prop_set_string(nvic, "cpu-type", machine->cpu_type);
    // qdev_prop_set_bit(nvic, "enable-bitband", true);
    // object_property_set_link(OBJECT(nvic), OBJECT(get_system_memory()),
    //                          "memory", &error_abort);
    // /* This will exit with an error if the user passed us a bad cpu_type */
    // qdev_init_nofail(nvic);

    // qdev_connect_gpio_out_named(nvic, "SYSRESETREQ", 0,
    //                             qemu_allocate_irq(&do_sys_reset, NULL, 0));

    // DeviceState *gpio_dev[NUM_OF_GPIOS];
    // for (unsigned i = 0; i < NUM_OF_GPIOS; ++i)
    // {

    //     gpio_dev[i] = sysbus_create_simple("feabhas", gpio_addr[i],
    //                                        qdev_get_gpio_in(nvic,
    //                                                         gpio_irq[i]));
    //     // for (j = 0; j < 8; j++)
    //     // {
    //     //     gpio_in[i][j] = qdev_get_gpio_in(gpio_dev[i], j);
    //     //     gpio_out[i][j] = NULL;
    //     // }
    // }

    armv7m_load_kernel(ARM_CPU(first_cpu),
                       machine->kernel_filename,
                       FLASH_SIZE);
}

static void feabhas_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "Feabhas Training Board Machine";
    mc->init = feabhas_init;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}

static const TypeInfo feabhas_type = {
    .name = MACHINE_TYPE_NAME("feabhas"),
    .parent = TYPE_MACHINE,
    .class_init = feabhas_class_init,
};

static void feabhas_machine_init(void)
{
    type_register_static(&feabhas_type);
}

type_init(feabhas_machine_init);

/////////////////////////////////////////////////////////////////////
// DEVICE
// #define TYPE_FEABHAS_GPIO "feabhas-gpio"
// #define FEABHAS_GPIO(obj) OBJECT_CHECK(FeabhasGPIOState, (obj), TYPE_GPIOKEY)

// #define STM32_GPIO_PIN_COUNT 16
// typedef struct FeabhasGPIOState
// {
//     SysBusDevice parent_obj;
//     MemoryRegion iomem;
//     qemu_irq irq[STM32_GPIO_PIN_COUNT];
// } feabhas_gpio_state_t;

// #include "migration/vmstate.h"
// static const VMStateDescription vmstate_feabhas_gpio = {
//     .name = "feabhas_gpio",
//     .version_id = 1,
//     .minimum_version_id = 1,
//     // .fields = (VMStateField[]){
//     //     VMSTATE_END_OF_LIST()}
// };

// static uint64_t stm32_gpio_read(void *opaque, uint64_t offset,
//                                 unsigned size)
// {
//     return 0;
// }
// static void stm32_gpio_write(void *opaque, uint64_t offset,
//                              uint64_t value, unsigned size)
// {
// }

// static const MemoryRegionOps stm32_gpio_ops = {
//     .read = stm32_gpio_read,
//     .write = stm32_gpio_write,
//     .valid.min_access_size = 4,
//     .valid.max_access_size = 4,
//     .endianness = DEVICE_NATIVE_ENDIAN};

// static void feabhas_gpio_init(Object *obj)
// {
//     DeviceState *dev = DEVICE(obj);
//     feabhas_gpio_state_t *s = OBJECT_CHECK(feabhas_gpio_state_t, dev, "feabhas-gpio");
//     SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

//     for (unsigned pin = 0; pin < STM32_GPIO_PIN_COUNT; ++pin)
//     {
//         sysbus_init_irq(sbd, &s->irq[pin]);
//     }
//     qdev_init_gpio_out(dev, s->irq, STM32_GPIO_PIN_COUNT);

//     memory_region_init_io(&s->iomem, obj, &stm32_gpio_ops, s,
//                           "gpio", 0x03ff);
//     sysbus_init_mmio(sbd, &s->iomem);
// }

// static void feabhas_gpio_class_init(ObjectClass *klass, void *data)
// {
//     DeviceClass *dc = DEVICE_CLASS(klass);
//     dc->vmsd = &vmstate_feabhas_gpio;
//     dc->desc = "Feabhas GPIO";
// }

// static const TypeInfo feabhas_gpio_info = {
//     .name = "feabhas-gpio",
//     .parent = TYPE_SYS_BUS_DEVICE,
//     .instance_size = sizeof(feabhas_gpio_state_t),
//     .instance_init = feabhas_gpio_init,
//     .class_init = feabhas_gpio_class_init,
// };

// static void feabhas_gpio_register_types(void)
// {
//     type_register_static(&feabhas_gpio_info);
// }

// type_init(feabhas_gpio_register_types)