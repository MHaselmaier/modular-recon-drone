CFLAGS = -DF_CPU=8000000 -Wall -Wextra -c -Os

TARGET=motor_controller
TARGET_ARCH=attiny25

SOURCES=motor_controller.c usi_twi_slave.c
OBJECTS_L=$(SOURCES:.c=_L.o)
OBJECTS_R=$(SOURCES:.c=_R.o)

all: hex

hex: $(TARGET)_L.hex $(TARGET)_R.hex 

$(TARGET)_L.hex: $(TARGET)_L.elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET)_L.elf $(TARGET)_L.hex

$(TARGET)_R.hex: $(TARGET)_R.elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET)_R.elf $(TARGET)_R.hex

$(TARGET)_L.elf: $(OBJECTS_L)
	avr-gcc -mmcu=$(TARGET_ARCH) $^ -o $@

$(TARGET)_R.elf: $(OBJECTS_R)
	avr-gcc -mmcu=$(TARGET_ARCH) $^ -o $@

%_L.o: %.c
	avr-gcc $(CFLAGS) -Daddress=0x01 -mmcu=$(TARGET_ARCH) $< -o $@

%_R.o: %.c
	avr-gcc $(CFLAGS) -Daddress=0x02 -mmcu=$(TARGET_ARCH) $< -o $@

clean:
	rm -f *.elf *.o *.hex

flash_L:
	avrdude -p t25 -c avrisp -b 19200 -P COM3 -U flash:w:motor_controller_L.hex

flash_R:
	avrdude -p t25 -c avrisp -b 19200 -P COM3 -U flash:w:motor_controller_R.hex