CXX = g++
CXXFLAGS = -O3

all: solucion1 solucion2

solucion1: solucion1.cpp
	$(CXX) $(CXXFLAGS) solucion1.cpp -o solucion1

solucion2: solucion2.cpp
	$(CXX) $(CXXFLAGS) solucion2.cpp -o solucion2

clean:
	del *.exe
