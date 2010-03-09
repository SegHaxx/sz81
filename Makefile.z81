# makefile for z81/xz81

# You need an ANSI C compiler. gcc is probably best.
CC=gcc

# these two only apply to xz81:
#
# -DSCALE=n sets how many times to scale up the window. You may need
#  to use SCALE=1 on older machines, but most things these days should
#  cope with SCALE=2 pretty reasonably. However, SCALE>1 doesn't work
#  on 1-bit or 4-bit displays at the moment.
# -DMITSHM should always be enabled unless you have problems compiling
#  with it on (xz81's rather slow without it).
#
XDEF=-DSCALE=2 -DMITSHM

# sound support, which if compiled in can be enabled with `-s' and/or
# `-a <type>'. Yes, don't worry, it's off by default. :-)
#
# comment this out if you're not compiling on Linux, or some other
# platform with the OSS API available.
#
SOUNDDEF=-DOSS_SOUND_SUPPORT

# this should point to where X is installed. (If you don't know where
# it is, try `/usr/X11', that's a common culprit.) On Linux it's
# generally here:
#
XROOT=/usr/X11R6

# xz81 has a rather complicated routine to draw a pixel (scaling makes
# it a bit non-trivial), which should be inlined by the compiler if
# possible. This definition is ok for gcc; if your compiler has
# problems, comment this out.
#
INLINEDEF=-DINLINE_DRAWPIX

# dest for make install
#
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
XBINDIR=$(BINDIR)
MANDIR=$(PREFIX)/man/man1
LIBDIR=$(PREFIX)/lib/z81

# if you want the X version to be installed in the usual X executables
# directory, uncomment this:
#
#XBINDIR=$(XROOT)/bin

# you shouldn't need to edit the rest
#-----------------------------------------------------------------

# this looks wrong, but *ops.c are actually #include'd by z80.c
Z81_OBJS=main.o common.o sound.o z80.o
XZ81_OBJS=xmain.o common.o sound.o z80.o

CFLAGS=-I$(XROOT)/include -O -Wall $(SOUNDDEF) \
	$(XDEF) $(INLINEDEF) -DLIBDIR=\"$(LIBDIR)\"

all: z81 xz81

# we make zx81get if z81 (the only Linux-specific one) is made.

z81: $(Z81_OBJS) zx81get
	$(CC) $(CFLAGS) -o z81 $(Z81_OBJS) -lvga

xz81: $(XZ81_OBJS)
	$(CC) $(CFLAGS) -o xz81 $(XZ81_OBJS) -L$(XROOT)/lib -lXext -lX11

zx81get: zx81get.o
	$(CC) $(CFLAGS) -o zx81get zx81get.o

z80.o: z80.c z80ops.c cbops.c edops.c


installdirs:
	/bin/sh ./mkinstalldirs $(BINDIR) $(XBINDIR) $(MANDIR) $(LIBDIR)

install: installdirs
	if [ -f z81 ]; then install -o root -m 4755 -s z81 $(BINDIR); fi
	if [ -f xz81 ]; then install -m 755 -s xz81 $(XBINDIR); fi
	if [ -f zx81get ]; then install -m 755 -s zx81get $(BINDIR); fi
	if [ -f zx80.rom ]; then install -m 644 zx80.rom $(LIBDIR); fi
	if [ -f zx81.rom ]; then install -m 644 zx81.rom $(LIBDIR); fi
	install -m 644 zx80kybd.pbm zx81kybd.pbm xz81.xpm $(LIBDIR)
	install -m 644 z81.1 zx81get.1 $(MANDIR)
	ln -sf $(MANDIR)/z81.1 $(MANDIR)/xz81.1

uninstall:
	$(RM) $(XBINDIR)/xz81
	$(RM) $(BINDIR)/z81 $(BINDIR)/zx81get
	$(RM) $(MANDIR)/z81.1* $(MANDIR)/xz81.1*
	$(RM) $(MANDIR)/zx81get.1*

clean:
	$(RM) *.o *~ xz81 z81 zx81get


# stuff to make distribution tgz

VERS=2.1

tgz: ../z81-$(VERS).tar.gz
  
../z81-$(VERS).tar.gz: clean
	@cd ..;ln -s z81 z81-$(VERS)
	cd ..;tar zchvf z81-$(VERS).tar.gz z81-$(VERS)
	@cd ..;$(RM) z81-$(VERS)
