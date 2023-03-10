COMPFLAGS = -std=c++11 -O3
LINKFLAGS = -I${MKLROOT}/include -L${MKLROOT}/lib 
LIBS = -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lm



main: main.cpp
	g++ $(COMPFLAGS) -o main main.cpp  $(LINKFLAGS) $(LIBS)

all: main

clean:
	rm a.out

