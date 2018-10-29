CFLAGS = -DF_CPU=8000000 -Wall -Wextra -c -Os -I "C:\\Program Files\\avr-gcc-7.2.0-x64-mingw\\avr\\include"
TARGET_ARCH = -mmcu=attiny25

SOURCES=motor_controll.c usiTwiSlave.c

OBJECTS = $(SOURCES:.c=.o)

all: hex

hex: motor_controll.hex

motor_controll.hex: motor_controll.elf
	avr-objcopy -O ihex -j .data -j .text motor_controll.elf motor_controll.hex

motor_controll.elf: $(OBJECTS)
	avr-gcc $(TARGET_ARCH) $(OBJECTS) -o motor_controll.elf

.c.o:
	avr-gcc $(CFLAGS) $(TARGET_ARCH) $< -o $@

clean:
	del *.elf *.o *.hex

flash:
	avrdude -p t25 -c avrisp -b 19200 -P COM3 -U flash:w:motor_controll.hex