# use g++ compiler
CXX=g++
CXXFLAGS?=-Wall -pedantic -std=c++11

# flag specifications for release and debug
RELEASEFLAGS?=$(CXXFLAGS) -O3
DEBUGFLAGS?=$(CXXFLAGS) -O0 -g #-pg

# executables
EXES=load_tree load_tree_topology print_num_descendants print_root_dists print_stats

# compile
all: $(EXES)
load_tree: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o load_tree load_tree.cpp
load_tree_topology: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o load_tree_topology load_tree_topology.cpp
print_num_descendants: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o print_num_descendants print_num_descendants.cpp
print_root_dists: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o print_root_dists print_root_dists.cpp
print_stats: compact_tree.h
	$(CXX) $(RELEASEFLAGS) -o print_stats print_stats.cpp
clean:
	$(RM) $(EXES) *.o
