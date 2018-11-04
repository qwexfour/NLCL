CXX = g++
DBGINFO = -g
CXXFLAGS = -lm -Wall -Werror -pedantic-errors --std=c++17 -O0

all: a.out

a.out: main.o mugratree.o
	$(CXX) $(CXXFLAGS) $(DBGINFO) main.o mugratree.o -o a.out

main.o: main.cpp mugratree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c main.cpp -o main.o

mugratree.o: mugratree.cpp mugratree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c mugratree.cpp -o mugratree.o

.PHONY: clean

clean:
	rm *.o a.out
