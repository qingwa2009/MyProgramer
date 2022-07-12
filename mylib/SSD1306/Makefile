###############################################################################
# Makefile for project 
###############################################################################
# LCD driver SPI or I2C
# DRIVER:=
# RST_PIN:=
# CS_PIN:=
# DC_PIN:=
# SSD1306_I2C_ADDRESS:=

## General options
PROJECT = 
MCU = ATmega328P
COMPILER = "Arduino AVR (Proteus)"
TARGET = Debug
SHELL = C:\Windows\system32\cmd.exe
OUT_DIR = ./bin
OPTIMIZE = Os

SRCS =$(wildcard *.c)
OBJS =$(SRCS:%.c=$(OUT_DIR)/%.o)
FNS = $(SRCS:%.c=%)

## Tools general options
ARDUINO_PATH="D:\Program Files\Proteus 8 Professional\Tools\ARDUINO"
mksketch = $(ARDUINO_PATH)\mksketch.exe

APPFLAGS=-C pro328 -T -std=gnu++11 -T -fno-threadsafe-statics -N atmega328p -F 16000000

CCFLAGS=-Wall -gdwarf-2 -fno-exceptions -DF_CPU=16000000 \
		-I . \
		-I $(ARDUINO_PATH)/"hardware/arduino/avr/cores/arduino" \
		-I $(ARDUINO_PATH)/"hardware/arduino/avr/variants/eightanaloginputs" \
		-DARDUINO_ARCH_AVR \
		-DARDUINO=181 \
		-$(OPTIMIZE) -mmcu=atmega328p

CFLAGS := $(CCFLAGS)

LDFLAGS = -Wl,--gc-sections -mmcu=atmega328p 
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
all:$(OUT_DIR) $(OBJS)

$(OUT_DIR): 
	-mkdir "$(OUT_DIR)"	

$(OUT_DIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< \
	-DSSD1306_$(DRIVER) -DRST_PIN=$(RST_PIN) -DCS_PIN=$(CS_PIN) -DDC_PIN=$(DC_PIN) \
	-DSSD1306_I2C_ADDRESS=$(SSD1306_I2C_ADDRESS)

# $(AR) $(ARFLAGS) $(OUT_DIR)/mylib.a $@

test:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(FNS)

clean:	
	rm -rf $(OUT_DIR)/*.o
