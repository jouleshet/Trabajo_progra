//Este es el makefile
CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11

TARGET1 = solucion1
TARGET2 = solucion2
TARGET3 = solucion3

DEPS = compara_ascii.cpp

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): solucion1.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) solucion1.cpp -o $(TARGET1)

$(TARGET2): solucion2.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) solucion2.cpp -o $(TARGET2)

$(TARGET3): solucion3.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) solucion3.cpp -o $(TARGET3)

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3)