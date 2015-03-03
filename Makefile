CXX = clang++-3.5
CXXFLAGS = -O3 -Wall -W -pedantic-errors -std=c++11 -msse4.2

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h *.hpp)

OBJECTS = $(SOURCES:%.cpp=%.o)

all: chess library

default: chess library

test: chess
	./chess --test

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

library: Engine.o Bishops.o BitBoard.o Board.o Kings.o Knights.o Move.o Pawns.o Queens.o Rooks.o
	ar cr BixNix.a Engine.o Bishops.o BitBoard.o Board.o Kings.o Knights.o Move.o Pawns.o Queens.o Rooks.o

chess: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

clean:
	-@rm -f core >/dev/null 2>&1
	-@rm -f chess >/dev/null 2>&1
	-@rm -f BixNix.a >/dev/null 2>&1
	-@rm -f $(OBJECTS) >/dev/null 2>&1

check-syntax:
	${CXX} ${CXXFLAGS} -o /dev/null -S ${CHK_SOURCES}

# Automatically generate dependencies and include them in Makefile
.depend: $(SOURCES) $(HEADERS)
	@echo "Generating dependencies"
	$(CXX) ${CXXFLAGS} -MM *.cpp > $@

-include .depend
# Put a dash in front of include when using gnu make.
# It stops gmake from warning us that the file
# doesn't exist yet, even though it will be properly
# made and included when all is said and done.
