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

The pins for the motor are located at `motor_controller.h`.
`main.c` contains the pins for to top and bottom switches.

# Install required packages

Debian:

```
make deps
```

Fedora:

```
make deps_dnf
```

Arch:

```
make deps_pacman
```

# Optional: Setup Language Server

Generate `compile_commands.json` for clangd

```
make clean
bear -- make build
```

# Compile and upload

```
make
```

# Read USB Serial (printf)

Warning: Can't upload and read at the same time.

```
screen /dev/ttyUSB0 115200
```
