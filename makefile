CXX = g++
DBGINFO = -g
CXXFLAGS = -lm -Wall -Werror -pedantic-errors --std=c++11 -O0

all: a.out

a.out: main.o
	$(CXX) $(CXXFLAGS) $(DBGINFO) main.o -o a.out

main.o: main.cpp mugratree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c main.cpp -o main.o

.PHONY: clean

clean:
	rm *.o a.out
