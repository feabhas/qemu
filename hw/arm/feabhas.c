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
#include "hw/arm/stm32f405_soc.h"
#include "hw/arm/boot.h"

static void feabhas_init(MachineState *machine)
{
    DeviceState *dev = qdev_create(NULL, TYPE_STM32F405_SOC);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
    object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);

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
#define TYPE_FEABHAS_GPIO "feabhas-gpio"
#define FEABHAS_GPIO(obj) OBJECT_CHECK(FeabhasGPIOState, (obj), TYPE_GPIOKEY)

#define STM32_GPIO_PIN_COUNT 16
typedef struct FeabhasGPIOState
{
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    qemu_irq irq[STM32_GPIO_PIN_COUNT];
} feabhas_gpio_state_t;

#include "migration/vmstate.h"
static const VMStateDescription vmstate_feabhas_gpio = {
    .name = "feabhas_gpio",
    .version_id = 1,
    .minimum_version_id = 1,
    // .fields = (VMStateField[]){
    //     VMSTATE_END_OF_LIST()}
};

static uint64_t stm32_gpio_read(void *opaque, uint64_t offset,
                                unsigned size)
{
    return 0;
}
static void stm32_gpio_write(void *opaque, uint64_t offset,
                             uint64_t value, unsigned size)
{
}

static const MemoryRegionOps stm32_gpio_ops = {
    .read = stm32_gpio_read,
    .write = stm32_gpio_write,
    .valid.min_access_size = 4,
    .valid.max_access_size = 4,
    .endianness = DEVICE_NATIVE_ENDIAN};

static void feabhas_gpio_init(Object *obj)
{
    DeviceState *dev = DEVICE(obj);
    feabhas_gpio_state_t *s = OBJECT_CHECK(feabhas_gpio_state_t, dev, "feabhas-gpio");
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    for (unsigned pin = 0; pin < STM32_GPIO_PIN_COUNT; ++pin)
    {
        sysbus_init_irq(sbd, &s->irq[pin]);
    }
    qdev_init_gpio_out(dev, s->irq, STM32_GPIO_PIN_COUNT);

    memory_region_init_io(&s->iomem, obj, &stm32_gpio_ops, s,
                          "gpio", 0x03ff);
    sysbus_init_mmio(sbd, &s->iomem);
}

static void feabhas_gpio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->vmsd = &vmstate_feabhas_gpio;
    dc->desc = "Feabhas GPIO";
}

static const TypeInfo feabhas_gpio_info = {
    .name = "feabhas-gpio",
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(feabhas_gpio_state_t),
    .instance_init = feabhas_gpio_init,
    .class_init = feabhas_gpio_class_init,
};

static void feabhas_gpio_register_types(void)
{
    type_register_static(&feabhas_gpio_info);
}

type_init(feabhas_gpio_register_types)