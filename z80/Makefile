CXXFLAGS = -Wall

.SUFFIXES: .c

.c.o:
	$(CXX) -g -c $(CXXFLAGS) -o $@ $<

ZOBS = zx81config.o z80.o z80_ops.o

z80test: z80test.o $(ZOBS)
	$(CXX) -g -o z80test z80test.o $(ZOBS)


z80test.o: z80test.c

zx81config.o: zx81config.c
z80.o: z80.c
z80_ops.o: z80_ops.c
