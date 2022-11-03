###############################################################################
# Makefile for project 
###############################################################################
## General options

#实现了cc2541跟atmega328的编程功能
# TARGET_MCU_FILE = mcu_cc2541.cpp
TARGET_MCU_FILE = mcu_atmega328.cpp

#必要的路径，必须短路径名，不然有空格会出错
ARDUINO_PATH = "D:\PROGRA~1\Arduino"
AVRDUDE_CONF_PATH = "$(ARDUINO_PATH)\hardware\tools\avr\etc\avrdude.conf"
mksketch = "buildTool\mksketch.exe"

UPLOAD_PORT = COM3
UPLOAD_BAUD = 115200
UPLOAD_PROGRAMER = arduino

## General options
PROJECT = 
COMPILER = "Arduino AVR (Proteus)"
TARGET = Debug
SHELL = C:\Windows\system32\cmd.exe
BUILD_DIR = debug


BOARD = pro328
MCU = ATmega328P
MMCU = atmega328p
F_CPU = 16000000
OPTIMIZE = Os

SRCS = $(wildcard *.c) 
OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))
THIS_DIR = $(abspath .)
CURR_FOLDER_NAME =$(lastword $(notdir $(THIS_DIR)))

MYLIB_PATH = mylib
MYLIB_OUT_PATH = $(BUILD_DIR)/mylib
MYLIB_OUT_PATH_RELATIVE_FROM_MYLIB = ../../$(CURR_FOLDER_NAME)/$(MYLIB_OUT_PATH)

## Tools general options
APPFLAGS=-C $(BOARD) -T -std=gnu++11 -T -fno-threadsafe-statics -N $(MMCU) -F $(F_CPU)

CCFLAGS=-Wall -gdwarf-2 -fno-exceptions -DF_CPU=$(F_CPU) \
		-I . \
		-DARDUINO_ARCH_AVR @$(BUILD_DIR)/arduino/libs.inc \
		-$(OPTIMIZE) -mmcu=$(MMCU)
CFLAGS := $(CCFLAGS)

LDFLAGS = -Wl,--gc-sections -mmcu=$(MMCU) \
		  -L $(MYLIB_OUT_PATH) -L $(BUILD_DIR) -L $(BUILD_DIR)/arduino	
##编译到boot区
# LDFLAGS += -Wl,--section-start=.text=0x3800
##指定代码中用__attribute__((section(".xxx")))修饰的函数编译到指定位置
# LDFLAGS += -Wl,--section=.xxx=0x3800

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


UPLOAD_FILE = $(BUILD_DIR)/Debug.hex

# Build tree:
all:	Debug

.PHONY: upload
upload: 
	avrdude -C$(AVRDUDE_CONF_PATH) -v \
	-p$(MCU) -c$(UPLOAD_PROGRAMER) -P$(UPLOAD_PORT) -b$(UPLOAD_BAUD) -D -Uflash:w:$(UPLOAD_FILE):i

Debug: $(BUILD_DIR)/Debug.elf 
	avr-size "$(BUILD_DIR)/Debug.ELF"

$(BUILD_DIR)/Debug.elf: $(BUILD_DIR)/main.o $(BUILD_DIR)/myprogramer.o $(BUILD_DIR)/mycc2541.o $(BUILD_DIR)/myATmega328.o $(BUILD_DIR)/targetmcu.o mylib.a 
	$(LD) $(LDFLAGS) -o $@ $(addprefix -l:,$(notdir $^)) -l:arduino.a -lm 
	avr-objcopy -O ihex -R .eeprom "$(BUILD_DIR)/Debug.elf" "$(BUILD_DIR)/Debug.hex"
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex "$(BUILD_DIR)/Debug.elf" "$(BUILD_DIR)/Debug.eep" || exit 0 
	avr-objcopy -O binary -R .eeprom "$(BUILD_DIR)/Debug.elf" "$(BUILD_DIR)/Debug.bin"
#bin文件没有代码的位置信息，所以起始地址都是从0开始：
#如果只有boot区，或者只app区，编译出来的文件就会小一点
#如果代码boot区跟app区都有，那么编译出来的文件将从0一直覆盖到boot区，文件偏大；

$(BUILD_DIR)/main.o: $(BUILD_DIR)/main.cpp 
	$(CC) $(CCFLAGS) -o $@ -c $<

$(BUILD_DIR)/myprogramer.o: myProgramer.cpp 
	$(CC) $(CCFLAGS) -o $@ -c $<

$(BUILD_DIR)/mycc2541.o: cc2541/mycc2541.c cc2541/mycc2541.h
	$(CC) $(CFLAGS) -o $@ -c $<	

$(BUILD_DIR)/myATmega328.o: atmega328/myATmega328.c atmega328/myATmega328.h
	$(CC) $(CFLAGS) -o $@ -c $<	

$(BUILD_DIR)/targetmcu.o: $(TARGET_MCU_FILE)
	$(CC) $(CCFLAGS) -o $@ -c $<	

$(BUILD_DIR)/main.cpp:	main.ino 
	-mkdir "$(BUILD_DIR)"
	$(APP) $(APPFLAGS) -A $(ARDUINO_PATH) -M $(BUILD_DIR)/arduino -O $@ $<
	make -f $(BUILD_DIR)/arduino/Makefile all

mylib.a: $(wildcard $(MYLIB_PATH)/*.c) $(wildcard $(MYLIB_PATH)/*.h) 
	@make -C $(MYLIB_PATH) -f Makefile all \
	OUT_DIR=$(MYLIB_OUT_PATH_RELATIVE_FROM_MYLIB) \
	ARDUINO_PATH=$(ARDUINO_PATH) \
	AVRDUDE_CONF_PATH=$(AVRDUDE_CONF_PATH) \
	mksketch=$(mksketch) \
	BOARD=$(BOARD) \
	MCU=$(MCU) \
	MMCU=$(MMCU) \
	F_CPU=$(F_CPU) \
	OPTIMIZE=$(OPTIMIZE)


# bootloader - attach bootloader to the firmware
bootloader:
	avr-objcopy -O ihex -R .eeprom "$(BUILD_DIR)/Debug.elf" "$(BUILD_DIR)/Debug.hex"
	copy /b bootloader.hex+"$(BUILD_DIR)/Debug.hex" "with.bootloader.hex"

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf $(BUILD_DIR)/main.d
# -make -f $(BUILD_DIR)/arduino/Makefile tidy

# cleanup - delete all generated files
clean:	tidy
	rm -rf $(BUILD_DIR)/Debug.elf
	rm -rf $(BUILD_DIR)/main.cpp
	rm -rf $(BUILD_DIR)/*.o
	rm -rf $(MYLIB_OUT_PATH)/*.o
	rm -rf $(MYLIB_OUT_PATH)/*.a
#-make -f $(BUILD_DIR)/arduino/Makefile clean

OBJTREE := $(BUILD_DIR)