SRC_DIR = src
BUILD_DIR = build

OPTIMIZATION = -Os

all: compile upload

compile:
	@mkdir -p $(BUILD_DIR)
	avr-gcc -c $(OPTIMIZATION) -DF_CPU=16000000UL -mmcu=atmega328p -o $(BUILD_DIR)/compiled.o $(SRC_DIR)/main.c
	avr-gcc -mmcu=atmega328p -o $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.o
	avr-objcopy -O ihex -R .eeprom $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.hex

upload:
	avrdude -v -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$(BUILD_DIR)/compiled.hex

size: compile
	avr-size --format=avr --mcu=atmega328p $(BUILD_DIR)/compiled.bin

asm:
	@mkdir -p $(BUILD_DIR)
	avr-gcc -S -Os -DF_CPU=16000000UL -mmcu=atmega328p -o $(BUILD_DIR)/compiled.asm $(SRC_DIR)/main.c

clean:
	rm -r $(BUILD_DIR)

.PHONY: upload size clean
