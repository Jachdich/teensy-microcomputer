# Teensyduino Core Library
# http://www.pjrc.com/teensy/
# Copyright (c) 2019 PJRC.COM, LLC.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# 1. The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# 2. If the Software is incorporated into a build system that allows
# selection among a list of target devices, then similar target
# devices manufactured by PJRC.COM must be included in the list of
# target devices and selectable in the same manner.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# set your MCU type here, or make command line `make MCU=IMXRT1062`
MCU=IMXRT1062

# The name of your project (used to name the compiled .hex file)
TARGET = main
#/usr/share/arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-g++ -E -CC -x c++ -w -g -Wall -ffunction-sections -fdata-sections -nostdlib -std=gnu++14 -fno-exceptions -fpermissive -fno-rtti -fno-threadsafe-statics -felide-constructors -Wno-error=narrowing -mthumb
#-mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -D__IMXRT1062__ -DTEENSYDUINO=153 -DARDUINO=10813 -DARDUINO_TEENSY41 -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -I/usr/share/arduino/hardware/teensy/avr/cores/teensy4
#/tmp/arduino_build_224829/sketch/Blink2.ino.cpp -o /dev/null -DARDUINO_LIB_DISCOVERY_PHASE

# configurable options
# OPTIONS = -DF_CPU=150000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE
OPTIONS = -DF_CPU=600 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE

# options needed by many Arduino libraries to configure for Teensy 4.0
OPTIONS += -D__$(MCU)__ -DARDUINO=10813 -DTEENSYDUINO=153 -DARDUINO_TEENSY41

# for Cortex M7 with single & double precision FPU
CPUOPTIONS = -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb

# use this for a smaller, no-float printf
#SPECS = --specs=nano.specs

# Other Makefiles and project templates for Teensy
#
# https://forum.pjrc.com/threads/57251?p=213332&viewfull=1#post213332
# https://github.com/apmorton/teensy-template
# https://github.com/xxxajk/Arduino_Makefile_master
# https://github.com/JonHylands/uCee


#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# Those that specify a NO_ARDUINO environment variable will
# be able to use this Makefile with no Arduino dependency.
# Please note that if ARDUINOPATH was set, it will override
# the NO_ARDUINO behaviour.
ifndef NO_ARDUINO
# Path to your arduino installation
ARDUINOPATH ?= /usr/share/arduino/
endif

ifdef ARDUINOPATH

# path location for Teensy Loader, teensy_post_compile and teensy_reboot (on Linux)
TOOLSPATH = tools

# path location for the arm-none-eabi compiler
COMPILERPATH = tools/arm/bin

else
# Default to the normal GNU/Linux compiler path if NO_ARDUINO
# and ARDUINOPATH was not set.
COMPILERPATH ?= /usr/bin

endif

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -O2 $(CPUOPTIONS) -MMD $(OPTIONS) -I. -ffunction-sections -fdata-sections -Iinclude -Iinclude/libs

# compiler options for C++ only
CXXFLAGS = -std=gnu++14 -felide-constructors -fno-exceptions -fpermissive -fno-rtti -Wno-error=narrowing

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections,--relax $(SPECS) $(CPUOPTIONS) -Timxrt1062_t41.ld

# additional libraries to link
LIBS = -larm_cortexM7lfsp_math -lm -lstdc++


# names for the compiler programs
CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

# automatically create lists of the sources and objects
# TODO: this does not handle Arduino libraries yet...
C_FILES := $(wildcard src/*.c)
CPP_FILES := $(wildcard src/*.cpp)
LIBC_FILES := $(wildcard src/libs/*.c)
LIBCPP_FILES := $(wildcard src/libs/*.cpp)
#OBJS := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)
SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LIBC_FILES:.c=.o) $(LIBCPP_FILES:.cpp=.o)
OBJS := $(patsubst src/%,obj/%,$(SOURCES))

obj/%.o: src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<
obj/%.o: src/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

obj/libs/%.o: src/libs/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<
obj/libs/%.o: src/libs/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<


all: $(TARGET).hex

$(TARGET).elf: $(OBJS) $(MCU_LD)
	@echo $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
ifneq (,$(wildcard $(TOOLSPATH)))
	$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	# teensy_loader_cli --mcu=TEENSY41 -w $(TARGET).hex
	# $(TOOLSPATH)/teensy_reboot
endif

# compiler generated dependency info
-include $(OBJS:.o=.d)

# make "MCU" lower case
LOWER_MCU := $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(MCU)))))))))))))))))))))))))))
MCU_LD = $(LOWER_MCU).ld

clean:
	rm -f obj/*.o obj/*.d obj/libs/*.o obj/libs/*.d $(TARGET).elf $(TARGET).hex
