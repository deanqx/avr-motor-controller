TODO Description

# Pins

The pins for the motor can be configured in `hal.h`.

# Install required packages

## For Arch Linux

```
sudo pacman -S cmake avr-gcc avrdude
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
