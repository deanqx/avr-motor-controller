This program was made to controll a stepper motor (moving linear)
for a Freefall Tower. The motor transports a loose cage
which falls when the motor hits a trigger at the bottom.
It has a switch at the top and the bottom to change the
direction of the motor. It waits for some time at the bottom.

# Features
- Controll Stepper Motor with L297 H-bridge
- Adjust the waiting time in a predefined range by poti
- LED indicating the adjusted speed by poti

# Pins

The pins for the motor can be configured in `hal.h`.

# Install required packages

## For Arch Linux

```
sudo pacman -Sy avr-gcc avrdude
```

# Build

## 1. Configure build system

This only needs to be done when `CMakeLists.txt` was changed.

```
cmake -B build
```

## 2. Build and upload sources

```
cmake --build build
```

## Build without upload

```
cmake --build build --target hex
```
