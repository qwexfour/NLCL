CXX = g++
DBGINFO = -g
CXXFLAGS = -lm -Wall -Werror -pedantic-errors --std=c++17 -O0

all: a.out

a.out: main.o forest.o
	$(CXX) $(CXXFLAGS) $(DBGINFO) main.o forest.o -o a.out

main.o: main.cpp forest.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c main.cpp -o main.o

forest.o: forest.cpp forest.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c forest.cpp -o forest.o

testnaivetree: testnaivetree.o 
	$(CXX) $(CXXFLAGS) $(DBGINFO) testnaivetree.o -o testnaivetree

testnaivetree.o: testnaivetree.cpp naivetree.hpp
	$(CXX) $(CXXFLAGS) $(DBGINFO) -c testnaivetree.cpp -o testnaivetree.o

.PHONY: clean

clean:
	rm *.o a.out
