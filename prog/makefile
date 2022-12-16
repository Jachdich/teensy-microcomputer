
.PHONY: all
.PHONY: clean

all: main.elf

main.elf: main.o
	../tools/arm/bin/arm-none-eabi-ld -T main.ld main.o -o main.elf

%.o: %.c
	../tools/arm/bin/arm-none-eabi-gcc -nostdlib -fPIC -fPIE -Wall -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb -MMD -DF_CPU=600 -D__IMXRT1062__ -ffunction-sections -fdata-sections -fno-exceptions -Wno-error=narrowing -ffreestanding -c -o $@ $^

clean:
	rm -f *.elf *.d *.o

