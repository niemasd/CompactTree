# use g++ compiler
CXX=g++
CXXFLAGS?=-Wall -pedantic -std=c++11

# flag specifications for release and debug
RELEASEFLAGS?=$(CXXFLAGS) -O3
DEBUGFLAGS?=$(CXXFLAGS) -O0 -g #-pg

# compile
all: test
test: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o test test.cpp
debug: compact_tree.h
	$(CXX) $(DEBUGFLAGS) -o test_debug test.cpp
clean:
	$(RM) test test_debug *.o
