CC = arm-none-eabi-gcc
CFLAGS = --std=gnu11 -Os -nostdlib -fPIC -msingle-pic-base -mpic-register=r9  -mcpu=cortex-m3 -mthumb -ffreestanding -nostartfiles -fno-builtin  --specs=nosys.specs --specs=nano.specs -Tkmod.ld -Wl,-N -Wl,--just-symbols=../../kernel-Mk1/src/kernel.elf
OBYCP = arm-none-eabi-objcopy

test.bin : test.c kmod.S
	$(CC) $(CFLAGS) -o test.elf test.c kmod.S
	$(OBYCP) -O binary test.elf test.bin
