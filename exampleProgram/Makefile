CC = arm-none-eabi-gcc
CFLAGS = --std=gnu11 -O0 -nostdlib -mlong-calls -fPIC -msingle-pic-base -mpic-register=r9  -mcpu=cortex-m3 -mthumb -fno-builtin  -ffreestanding   --specs=nosys.specs --specs=nano.specs -Tprog.ld -Wl,-N
OBYCP = arm-none-eabi-objcopy

INCLUDE = -I ../../kernel-Mk1/src/

test.bin : ex1.c prog.S
	$(CC) $(CFLAGS) -o ex1.elf ex1.c prog.S $(INCLUDE)
	$(OBYCP) -O binary ex1.elf ex1.bin
