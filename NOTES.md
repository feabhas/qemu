In  build dir
```
../configure  --enable-debug --target-list="arm-softmmu"
make
```

added `--trace events=trace-event-list` for `qemu` invocation, e.g.
```
/Users/Niall/Documents/programming/qemu/qemu/build/arm-softmmu/qemu-system-arm -S -M feabhas -s -d unimp,guest_errors -kernel  build/debug/C-501.elf -monitor tcp::55555,server --trace events=trace-event-list
```

Trace file:
```
stm32f40x_gpio_*
```

then output is:
```
23889@1585081275.768789:stm32f40x_gpio_write offset 0x0 value 0x550000
23889@1585081301.171452:stm32f40x_gpio_read offset 0x14 value 0x0
23889@1585081301.173188:stm32f40x_gpio_write offset 0x14 value 0x800
23889@1585081310.611429:stm32f40x_gpio_read offset 0x14 value 0x800
23889@1585081310.613418:stm32f40x_gpio_write offset 0x14 value 0xc00
23889@1585081312.444239:stm32f40x_gpio_read offset 0x14 value 0xc00
23889@1585081312.446240:stm32f40x_gpio_write offset 0x14 value 0xe00
23889@1585081313.933223:stm32f40x_gpio_read offset 0x14 value 0xe00
23889@1585081313.935520:stm32f40x_gpio_write offset 0x14 value 0xf00
23889@1585081316.731271:stm32f40x_gpio_read offset 0x14 value 0xf00
23889@1585081316.733111:stm32f40x_gpio_write offset 0x14 value 0x700
23889@1585081404.923014:stm32f40x_gpio_read offset 0x14 value 0x700
23889@1585081404.924907:stm32f40x_gpio_write offset 0x14 value 0x300
23889@1585081406.616402:stm32f40x_gpio_read offset 0x14 value 0x300
23889@1585081406.618288:stm32f40x_gpio_write offset 0x14 value 0x100
23889@1585081408.485612:stm32f40x_gpio_read offset 0x14 value 0x100
23889@1585081408.487301:stm32f40x_gpio_write offset 0x14 value 0x0
```
