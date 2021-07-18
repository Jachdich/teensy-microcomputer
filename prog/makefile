
.PHONY: all
.PHONY: clean

all: main.elf

%.elf: %.c
	../tools/arm/bin/arm-none-eabi-g++ -nostartfiles -fPIC -fPIE -Wall -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb -MMD -DF_CPU=600 -D__IMXRT1062__ -ffunction-sections -fdata-sections -std=gnu++14 -felide-constructors -fno-exceptions -fpermissive -fno-rtti -Wno-error=narrowing -ffreestanding -o $@ $^

clean:
	rm -f *.elf *.d *.o