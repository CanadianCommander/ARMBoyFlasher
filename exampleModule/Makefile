CC = arm-none-eabi-gcc
CFLAGS = --std=gnu11 -O0 -nostdlib -mlong-calls -fPIC -msingle-pic-base -mpic-register=r9  -mcpu=cortex-m3 -mthumb -fno-builtin  -ffreestanding   --specs=nosys.specs --specs=nano.specs -Tkmod.ld -Wl,-N -Wl,--just-symbols=../../kernel-Mk1/src/kernel.elf
OBYCP = arm-none-eabi-objcopy

INCLUDE = -I ../../kernel-Mk1/src/

test.bin : test.c kmod.S
	$(CC) $(CFLAGS) -o test.elf test.c kmod.S $(INCLUDE)
	$(OBYCP) -O binary test.elf test.bin
