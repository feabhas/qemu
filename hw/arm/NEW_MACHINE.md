# Adding a new base machine

## type_init(init fn ptr) - only extern 
1. <board>_machine_init
2. expands to ```do_qemu_init_ ## function```

```
type_init(feabhas_machine_init)
```

## static <board>_machine_init(void)
   1. calls type_register_static(const TypeInfo*)

```
static void feabhas_machine_init(void)
{
    type_register_static(&feabhas_type);
}
```

## TypeInfo instance

struct defined in `"object.h"`

   1. .name = MACHINE_TYPE_NAME("string")  // expands to string+" -machine"
   2. .parent = TYPE_MACHINE    // expand to "machine"
   3. .instance_size = [optional] used for stateful behaviour
      1. This looks to be replaced by separate inits for devices (ADC, etc.)
   4. .class_init = fn ptr to machine class init fn
      1. Data to pass to the @class_init, @class_base_init. This can be useful when building dynamic classes.

```
static const TypeInfo feabhas_type = {
    .name = MACHINE_TYPE_NAME("feabhas"),
    .parent = TYPE_MACHINE,
    .class_init = feabhas_class_init,
};
```


## <board>_machine_init

Sets up the MachineClass structure from `boards.h`
Minimum seems to be:
1. MachineClass *mc = MACHINE_CLASS(oc); // macro in boards.h
2. mc->desc = "Name fo manchine"
3. mc->init = <fn ptr to machine_init fn>

Other options
* mc->max_cpus = 1; // microbit - but default is 1?
* mc->ignore_memory_transaction_failures = true; // stellaris but don't use
  * New board models should instead use "unimplemented-device" for all memory ranges where the guest will attempt to probe for a device that QEMU doesn't implement and a stub device is required.
* mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m3");
  * specifies default CPU_TYPE, which will be used for parsing target specific features and for creating CPUs if CPU name wasn't provided explicitly at CLI
  * ARM_CPU_TYPE_NAME("cortex-m3") macro from `cpu.h` expands to "cortex-m3-arm-cpu"

```
static void feabhas_machine_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "Feabhas Training Board Machine";
    mc->init = feabhas_init;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
}
```

## <board>_init
```
static void feabhas_init(MachineState *machine)
{
    DeviceState *dev = qdev_create(NULL, TYPE_STM32F405_SOC);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
    object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);

    armv7m_load_kernel(ARM_CPU(first_cpu),
                       machine->kernel_filename,
                       FLASH_SIZE);
}
```


# Building

qemu/build/config-all-devices.mak:
  223: CONFIG_NETDUINOPLUS2:=$(findstring y,$(CONFIG_NETDUINOPLUS2)y)
qemu/build/config-all-devices.mak
```
CONFIG_FEABHAS:=$(findstring y,$(CONFIG_FEABHAS)y)
```

qemu/build/aarch64-softmmu/config-devices.h:
  115: #define CONFIG_NETDUINOPLUS2 1
qemu/build/aarch64-softmmu/config-devices.h
```
#define CONFIG_FEABHAS 1
```

qemu/build/aarch64-softmmu/config-devices.mak:
  114: CONFIG_NETDUINOPLUS2=y
qemu/build/aarch64-softmmu/config-devices.mak
```
CONFIG_FEABHAS=y
```

qemu/build/aarch64-softmmu/config-devices.mak.old:
  114: CONFIG_NETDUINOPLUS2=y


build/arm-softmmu/config-devices.h:
  113: #define CONFIG_NETDUINOPLUS2 1
qemu/build/arm-softmmu/config-devices.h
```
#define CONFIG_FEABHAS 1
```




qemu/build/arm-softmmu/config-devices.mak.old:
  112: CONFIG_NETDUINOPLUS2=y
qemu/build/arm-softmmu/config-devices.mak
```
CONFIG_FEABHAS=y
```

qemu/build/arm-softmmu/config-devices.mak:
  112: CONFIG_NETDUINOPLUS2=y
qemu/build/arm-softmmu/config-devices.mak.old
```
CONFIG_FEABHAS=y
```

qemu/default-configs/arm-softmmu.mak:
  33: CONFIG_NETDUINOPLUS2=y
qemu/default-configs/arm-softmmu.mak
```
CONFIG_FEABHAS=y
```
hw/arm/Kconfig:
  106: config NETDUINOPLUS2
hw/arm/Kconfig
```
config FEABHAS
    bool
    select STM32F405_SOC
```
hw/arm/Makefile.objs:
  14: obj-$(CONFIG_NETDUINOPLUS2) += netduinoplus2.o
hw/arm/Makefile.objs
```
obj-$(CONFIG_FEABHAS) += feabhas.o
```

## Make

```
 % ./arm-softmmu/qemu-system-arm -machine help
Supported machines are:
akita                Sharp SL-C1000 (Akita) PDA (PXA270)
ast2500-evb          Aspeed AST2500 EVB (ARM1176)
ast2600-evb          Aspeed AST2600 EVB (Cortex A7)
borzoi               Sharp SL-C3100 (Borzoi) PDA (PXA270)
canon-a1100          Canon PowerShot A1100 IS
cheetah              Palm Tungsten|E aka. Cheetah PDA (OMAP310)
collie               Sharp SL-5500 (Collie) PDA (SA-1110)
connex               Gumstix Connex (PXA255)
cubieboard           cubietech cubieboard (Cortex-A8)
emcraft-sf2          SmartFusion2 SOM kit from Emcraft (M2S010)
feabhas              Feabhas Training Board Machine
highbank             Calxeda Highbank (ECX-1000)
imx25-pdk            ARM i.MX25 PDK board (ARM926)
integratorcp         ARM Integrator/CP (ARM926EJ-S)
```