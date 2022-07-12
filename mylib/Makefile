###############################################################################
# Makefile for project 
###############################################################################
ARDUINO_PATH = "D:\PROGRA~1\PROTEU~1\Tools\ARDUINO"
AVRDUDE_CONF_PATH = "D:\PROGRA~1\Arduino\hardware\tools\avr\etc\avrdude.conf"
mksketch = $(ARDUINO_PATH)"\mksketch.exe"

## General options
PROJECT = 
COMPILER = "Arduino AVR (Proteus)"
TARGET = Debug
SHELL = C:\Windows\system32\cmd.exe
BUILD_DIR = bin

BOARD = pro328
MCU = ATmega328P
MMCU = atmega328p
F_CPU = 16000000
OPTIMIZE = Os

SRCS =$(wildcard *.c)
OBJS =$(SRCS:%.c=$(OUT_DIR)/%.o)
SRCSCPP =$(wildcard *.cpp)
OBJSCPP =$(SRCSCPP:%.cpp=$(OUT_DIR)/%.o)
FNS = $(SRCS:%.c=%)

## Tools general options

APPFLAGS=-C $(BOARD) -T -std=gnu++11 -T -fno-threadsafe-statics -N $(MMCU) -F $(F_CPU)

CCFLAGS=-Wall -gdwarf-2 -fno-exceptions -DF_CPU=$(F_CPU) \
		-I . \
		-I $(ARDUINO_PATH)"/hardware/arduino/avr/cores/arduino" \
		-I $(ARDUINO_PATH)"/hardware/arduino/avr/variants/eightanaloginputs" \
		-DARDUINO_ARCH_AVR \
		-DARDUINO=181 \
		-$(OPTIMIZE) -mmcu=$(MMCU)

CFLAGS := $(CCFLAGS)

LDFLAGS = -Wl,--gc-sections -mmcu=$(MMCU) 
##编译到boot区
# LDFLAGS += -Wl,--section-start=.text=0x3800

CCFLAGS += -std=gnu++11 -fno-threadsafe-statics
CFLAGS += -std=gnu11

## for Release
CCFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -ffunction-sections -fdata-sections

## Processing Tools
FLOWCHART=
APP=$(mksketch)
CC=avr-gcc
ASM=avr-gcc
LD=avr-gcc

AR=ar
ARFLAGS = ru

.PHONY: test MKDIR

# Build tree:
all:$(OUT_DIR) $(OBJS) $(OBJSCPP)

$(OUT_DIR): 
	-mkdir "$(OUT_DIR)"	

$(OUT_DIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< 
	$(AR) $(ARFLAGS) $(OUT_DIR)/mylib.a $@

$(OUT_DIR)/%.o: %.cpp %.h
	$(CC) $(CCFLAGS) -o $@ -c $< 
	$(AR) $(ARFLAGS) $(OUT_DIR)/mylib.a $@

test:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(SRCSCPP)
	@echo $(OBJSCPP)
	@echo $(FNS)

clean:	
	rm -rf $(OUT_DIR)/*.o
	rm -rf $(OUT_DIR)/*.a
