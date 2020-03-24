# Part 1 
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

# Part 2