# use g++ compiler
CXX=g++
CXXFLAGS?=-Wall -pedantic -std=c++11

# flag specifications for release and debug
RELEASEFLAGS?=$(CXXFLAGS) -O3
DEBUGFLAGS?=$(CXXFLAGS) -O0 -g #-pg

# executables
EXES=load_tree

# compile
all: $(EXES)
load_tree: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o load_tree load_tree.cpp
clean:
	$(RM) $(EXES) *.o
