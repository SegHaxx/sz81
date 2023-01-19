CCFLAGS=-O3 -Wall
CXXFLAGS=-O3 -Wall -DUSE_SND_EXTERNAL_BUFFER

SOURCES = sndrender.cpp sndcounter.cpp sndbuffer.cpp sndchip.cpp sndinterface.cpp
OBJECTS=$(patsubst %.cpp, %.o, $(SOURCES))

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

libsndrender.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o *.a *~
