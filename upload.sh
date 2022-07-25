#!/bin/sh

FIRMWARE="/tmp/Deviot/polivator/.pio/build/pro16MHzatmega328/firmware.hex"
# pro16MHzatmega328
avrdude -v -patmega328p -cusbasp -Pusb -Uflash:w:${FIRMWARE}:i

# Erase chip
# avrdude -v -patmega328p -cusbasp -Pusb -e

# Read fuse
# avrdude -v -patmega328p -cusbasp -Pusb -U lfuse:r:-:i
# avrdude -v -patmega328p -cusbasp -Pusb -U hfuse:r:-:i
# avrdude -v -patmega328p -cusbasp -Pusb -U efuse:r:-:i

# avrdude: safemode: Fuses OK (E:FD, H:DA, L:FF)


