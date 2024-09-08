SRC_DIR = src
BUILD_DIR = build

default:
	@mkdir -p $(BUILD_DIR)
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o $(BUILD_DIR)/compiled.o $(SRC_DIR)/main.c
	avr-gcc -o $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.o
	avr-objcopy -O ihex -R .eeprom $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$(BUILD_DIR)/compiled.hex

clean:
	rm -r $(BUILD_DIR)
