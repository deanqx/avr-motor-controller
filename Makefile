SRC_DIR = src
BUILD_DIR = build

OPTIMIZATION = -Os

port ?= /dev/ttyUSB0
i ?= $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(i))

$(shell	@mkdir -p $(BUILD_DIR))

all: compile upload

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	avr-gcc $(OPTIMIZATION) -DF_CPU=16000000UL -mmcu=atmega328p -c $< -o $@

compile: $(OBJS)
	avr-gcc -mmcu=atmega328p -o $(BUILD_DIR)/compiled.bin $(OBJS)
	avr-objcopy -O ihex -R .eeprom $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.hex

upload:
	avrdude -c arduino -p ATMEGA328P -P $(port) -b 115200 -U flash:w:$(BUILD_DIR)/compiled.hex

size: compile
	avr-size --format=avr --mcu=atmega328p $(BUILD_DIR)/compiled.bin

asm:
	@mkdir -p $(BUILD_DIR)
	avr-gcc -S -Os -DF_CPU=16000000UL -mmcu=atmega328p -o $(BUILD_DIR)/compiled.asm $(SRC_DIR)/main.c

clean:
	rm compile_commands.json
	rm -r $(BUILD_DIR)

.PHONY: upload size clean
