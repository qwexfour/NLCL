CXX = g++
DBGINFO = -g
CXXFLAGS = -lm -Wall -Werror -pedantic-errors --std=c++17 -O0

all: a.out

a.out: main.o 
	$(CXX) $(CXXFLAGS) $(DBGINFO) main.o -o a.out

main.o: main.cpp tree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c main.cpp -o main.o

testnaivetree: testnaivetree.o 
	$(CXX) $(CXXFLAGS) $(DBGINFO) testnaivetree.o -o testnaivetree

testnaivetree.o: testnaivetree.cpp naivetree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c testnaivetree.cpp -o testnaivetree.o

.PHONY: clean

clean:
	rm *.o a.out
