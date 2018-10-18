CXX = avr-g++
CPPFLAGS = -DF_CPU=1000000
CPPFLAGS += -I./include
CXXFLAGS = -Wall -Wextra -fconcepts -std=c++17 -Os -I "C:\\Program Files\\avr-gcc-7.2.0-x64-mingw\\avr\\include"
TARGET_ARCH = -mmcu=attiny25


all: motor_controll.elf

motor_controll.elf: motor_controll.cpp

# $< ::= %.cpp; $@ :== %.elf
%.elf: %.cpp
	$(CXX) $(TARGET_ARCH) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean:
	del *.elf

flash:
	avrdude -p t25 -c avrisp -b 19200 -P COM3 -U flash:w:motor_controll.elf