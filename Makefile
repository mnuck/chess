CXX = g++
CXXFLAGS = -O3 -Wall -W -pedantic-errors -std=c++11 -msse4.2

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h *.hpp)

OBJECTS = $(SOURCES:%.cpp=%.o)

all: chess

default: chess

test: chess
	./chess test.board

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

chess: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

clean:
	-@rm -f core >/dev/null 2>&1
	-@rm -f chess >/dev/null 2>&1
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
