OS=Linux

ifeq ($(OS), Windows)
	CFLAGS = -DF_CPU=8000000 -Wall -Wextra -c -Os -I "C:\\Program Files\\avr-gcc-7.2.0-x64-mingw\\avr\\include"
endif
ifeq ($(OS), Linux)
	CFLAGS = -DF_CPU=8000000 -Wall -Wextra -c -Os
endif

TARGET=motor_controller
TARGET_ARCH=attiny25

SOURCES=motor_controller.c usi_twi_slave.c
OBJECTS=$(SOURCES:.c=.o)

all: hex

hex: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	avr-gcc -mmcu=$(TARGET_ARCH) $^ -o $@

%.o: %.c
	avr-gcc $(CFLAGS) -mmcu=$(TARGET_ARCH) $< -o $@

clean:
	rm -f *.elf *.o *.hex

flash:
	avrdude -p t25 -c avrisp -b 19200 -P COM3 -U flash:w:motor_controller.hex