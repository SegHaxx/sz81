
TARGET=pipepanic

.PHONY: all

all:
	-@if [ -n "`which z80-asm 2> /dev/null`" ]; then \
		echo "Z80-ASM found in `which z80-asm`" ; \
		z80-asm $(TARGET).asm 0x4009: ; \
		dd ibs=10 skip=1 if=$(TARGET).z80 of=$(TARGET).p ; \
		rm $(TARGET).z80 ; \
	elif [ -n "`which pasmo 2> /dev/null`" ]; then \
		echo "Pasmo found in `which pasmo`" ; \
		sed -i.orig -e 's/\tcond/\tif/' -e 's/\tendc/\tendif/' $(TARGET).asm ; \
		pasmo -v $(TARGET).asm $(TARGET).p ; \
		mv -f $(TARGET).asm.orig $(TARGET).asm ; \
	else \
		echo "The $(TARGET) source code was written for the Z80-ASM cross-assembler," ; \
		echo "and this Makefile adds support for Pasmo; unfortunately you have neither." ; \
	fi

