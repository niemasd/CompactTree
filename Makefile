# use g++ compiler
CXX=g++
CXXFLAGS?=-Wall -pedantic -std=c++11

# flag specifications for release and debug
RELEASEFLAGS?=$(CXXFLAGS) -O3
DEBUGFLAGS?=$(CXXFLAGS) -O0 -g #-pg

# executables
EXES=copy_tree load_tree load_tree_string print_distance_matrix print_distances print_mrca print_num_descendants print_root_dists print_stats print_subtree_mrca print_topology

# compile
all: $(EXES)
copy_tree: compact_tree.h example/copy_tree.cpp
	$(CXX) $(RELEASEFLAGS) -o copy_tree example/copy_tree.cpp
load_tree: compact_tree.h example/load_tree.cpp
	$(CXX) $(RELEASEFLAGS) -o load_tree example/load_tree.cpp
load_tree_string: compact_tree.h example/load_tree_string.cpp
	$(CXX) $(RELEASEFLAGS) -o load_tree_string example/load_tree_string.cpp
print_distance_matrix: compact_tree.h example/print_distance_matrix.cpp
	$(CXX) $(RELEASEFLAGS) -o print_distance_matrix example/print_distance_matrix.cpp
print_distances: compact_tree.h example/print_distances.cpp
	$(CXX) $(RELEASEFLAGS) -o print_distances example/print_distances.cpp
print_mrca: compact_tree.h example/print_mrca.cpp
	$(CXX) $(RELEASEFLAGS) -o print_mrca example/print_mrca.cpp
print_num_descendants: compact_tree.h example/print_num_descendants.cpp
	$(CXX) $(RELEASEFLAGS) -o print_num_descendants example/print_num_descendants.cpp
print_root_dists: compact_tree.h example/print_root_dists.cpp
	$(CXX) $(RELEASEFLAGS) -o print_root_dists example/print_root_dists.cpp
print_stats: compact_tree.h example/print_stats.cpp
	$(CXX) $(RELEASEFLAGS) -o print_stats example/print_stats.cpp
print_subtree_mrca: compact_tree.h example/print_subtree_mrca.cpp
	$(CXX) $(RELEASEFLAGS) -o print_subtree_mrca example/print_subtree_mrca.cpp
print_topology: compact_tree.h example/print_topology.cpp
	$(CXX) $(RELEASEFLAGS) -o print_topology example/print_topology.cpp
clean:
	$(RM) -r $(EXES) *.o html latex
