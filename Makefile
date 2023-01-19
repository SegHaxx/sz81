# Comment/uncomment these to choose an installation destination
# System wide installation
#PREFIX?=/usr/local
#BINDIR?=$(PREFIX)/bin
#DOCDIR?=$(PREFIX)/share/doc/$(TARGET)
#PACKAGE_DATA_DIR?=$(PREFIX)/share/$(TARGET)

# Local installation within your home folder
#PREFIX?=$(HOME)/Games/$(TARGET)
#BINDIR?=$(PREFIX)
#DOCDIR?=$(PREFIX)/doc
#PACKAGE_DATA_DIR?=$(PREFIX)/data

# Run from current folder i.e. no installation
PREFIX?=.
BINDIR?=$(PREFIX)
DOCDIR?=$(PREFIX)
PACKAGE_DATA_DIR?=$(PREFIX)/data

# For sz81 OSS_SOUND_SUPPORT is now synonymous with SDL_SOUND_SUPPORT.
# Comment this out if you don't want sound support.
SOUNDDEF=-DOSS_SOUND_SUPPORT

# You won't need to alter these
TARGET=$(shell cat TARGET)
SOURCES=sdl_main.c common.c sound.c zx81config.c w5100.c sdl_engine.c sdl_hotspots.c \
	sdl_input.c sdl_loadsave.c sdl_resources.c sdl_sound.c sdl_video.c \
	z80/z80.c z80/z80_ops.c zx81.c dissz80.c tables.c noscript.c

OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
VERSION=$(shell cat VERSION)

# These should be ok for most.
# For debugging/profiling uncomment the following two lines:
#CFLAGS=-O0 -g -pg
#LDFLAGS=-pg
SDL_CONFIG?=sdl-config
CFLAGS?=-O3
CFLAGS+=-Wall -Wno-unused-result `$(SDL_CONFIG) --cflags` -DVERSION=\"$(VERSION)\" -DENABLE_EMULATION_SPEED_ADJUST \
	-DPACKAGE_DATA_DIR=\"$(PACKAGE_DATA_DIR)\" $(SOUNDDEF) -DSZ81 -D_DZ80_EXCLUDE_SCRIPT
# options:
# -DAPU
# -DZXPAND
# -DZXNU
# -DZXMORE (-DZXMSHMEM)
# -DZXMROML=0xF8 -DZXMRAML=0x13
# -DVDRIVE

LINK=$(CC)
#LDFLAGS=
LIBS=`$(SDL_CONFIG) --libs` -lrt -Lsndrender -lsndrender -Lzxpand -lzxpand

# You won't need to alter anything below
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) sndrender/libsndrender.a zxpand/libzxpand.a am9511/am9511.o
	g++ $(LDFLAGS) $(OBJECTS) $(LIBS) am9511/am9511.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

sndrender/libsndrender.a: sndrender/sndbuffer.cpp sndrender/sndchip.cpp sndrender/sndcounter.cpp sndrender/sndrender.cpp sndrender/sndinterface.cpp
	cd sndrender && $(MAKE)

zxpand/libzxpand.a: zxpand/zxpand_emu.cpp zxpand/zxpandclass.cpp zxpand/smbsd.cpp zxpand/zxpandcom.cpp zxpand/usart.cpp zxpand/zxpandcore.cpp zxpand/js.cpp zxpand/wildcard.cpp zxpand/ff.cpp
	cd zxpand && CXXFLAGS='$(CFLAGS) -Wno-trigraphs' $(MAKE)

am9511/am9511.o: am9511/am9511.cpp
	cd am9511 && $(CXX) -c $(CFLAGS) am9511.cpp

.PHONY: all clean install

open%:
	-@if [ -n "`which pasmo 2> /dev/null`" ]; then \
		pasmo -v open8x/$@.asm open8x/$@.rom; \
		if [ -f open8x/$@.rom -a ! -e data/zx$*.rom ]; then \
			cp open8x/$@.rom data/zx$*.rom; \
		fi \
	else \
		echo "The Pasmo cross-assembler was not found: skipping $@"; \
	fi

clean:
	cd sndrender && $(MAKE) clean
	cd zxpand && $(MAKE) clean
	rm -f *.o *~ sz81 z80/*.o z80/*~ am9511/am9511.o stzxfs

install:
	@if [ "$(PREFIX)" = . ] ; then \
		echo "Installing into the current folder is not allowed."; \
		exit 2; \
	fi
	mkdir -p $(BINDIR)
	mkdir -p $(DOCDIR)
	mkdir -p $(PACKAGE_DATA_DIR)
	cp $(TARGET) $(BINDIR)
	cp AUTHORS COPYING ChangeLog NEWS README $(DOCDIR)
	cp data/*.bmp $(PACKAGE_DATA_DIR)
	@if [ -f data/zx80.rom ]; then cp data/zx80.rom $(PACKAGE_DATA_DIR); fi
	@if [ -f data/zx81.rom ]; then cp data/zx81.rom $(PACKAGE_DATA_DIR); fi
	@if [ -f data/aszmic.rom ]; then cp data/aszmic.rom $(PACKAGE_DATA_DIR); fi
	@if [ -f data/h4th.rom ]; then cp data/h4th.rom $(PACKAGE_DATA_DIR); fi
	@if [ -f data/zx81font.rom ]; then cp data/zx81font.rom $(PACKAGE_DATA_DIR); fi
	@if [ -f data/zx81.zxpand.ovl ]; then cp data/zx81.zxpand.ovl $(PACKAGE_DATA_DIR); fi
	@if [ -f data/zxnu.rom ]; then cp data/zxnu.rom $(PACKAGE_DATA_DIR); fi

uninstall:
	@echo "Uninstalling is not currently implemented."


