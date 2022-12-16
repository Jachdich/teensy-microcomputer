MCU=IMXRT1062
TARGET = main
OPTIONS = -DF_CPU=600
CPUOPTIONS = -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb
CPPFLAGS = -Wall -g -O2 $(CPUOPTIONS) -MMD $(OPTIONS) -I. -ffunction-sections -fdata-sections -Iinclude
LDFLAGS = -Os -Wl,--gc-sections,--relax $(SPECS) $(CPUOPTIONS) -Timxrt1062_t41.ld
LIBS = -larm_cortexM7lfsp_math -lm

TOOLSPATH = tools
COMPILERPATH = tools/arm/bin

CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

C_FILES := $(wildcard src/*.c)
SOURCES := $(C_FILES:.c=.o)
OBJS := $(patsubst src/%,obj/%,$(SOURCES))

obj/%.o: src/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

all: $(TARGET).hex

$(TARGET).elf: $(OBJS) $(MCU_LD)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	#teensy_loader_cli --mcu=TEENSY41 -w $(TARGET).hex
	#$(TOOLSPATH)/teensy_reboot

# compiler generated dependency info
-include $(OBJS:.o=.d)

# make "MCU" lower case
LOWER_MCU := $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(MCU)))))))))))))))))))))))))))
MCU_LD = $(LOWER_MCU).ld

clean:
	rm -f obj/*.o obj/*.d $(TARGET).elf $(TARGET).hex
