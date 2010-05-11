TARGET=mctemplate

.PHONY: all

all:
	z80-asm $(TARGET).asm 0x4009:
	dd ibs=10 skip=1 if=$(TARGET).z80 of=$(TARGET).p
	rm $(TARGET).z80
