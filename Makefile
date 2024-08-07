# use g++ compiler
CXX=g++
CXXFLAGS?=-Wall -pedantic -std=c++11

# flag specifications for release and debug
RELEASEFLAGS?=$(CXXFLAGS) -O3
DEBUGFLAGS?=$(CXXFLAGS) -O0 -g #-pg

# executables
EXES=copy_tree load_tree load_tree_string print_distance_matrix print_distances print_mrca print_node_data print_num_descendants print_root_dists print_stats print_subtree_mrca print_topology

# compile
all: $(EXES)
copy_tree: CompactTree/compact_tree.h example/copy_tree.cpp
	$(CXX) $(RELEASEFLAGS) -o copy_tree example/copy_tree.cpp
load_tree: CompactTree/compact_tree.h example/load_tree.cpp
	$(CXX) $(RELEASEFLAGS) -o load_tree example/load_tree.cpp
load_tree_string: CompactTree/compact_tree.h example/load_tree_string.cpp
	$(CXX) $(RELEASEFLAGS) -o load_tree_string example/load_tree_string.cpp
print_distance_matrix: CompactTree/compact_tree.h example/print_distance_matrix.cpp
	$(CXX) $(RELEASEFLAGS) -o print_distance_matrix example/print_distance_matrix.cpp
print_distances: CompactTree/compact_tree.h example/print_distances.cpp
	$(CXX) $(RELEASEFLAGS) -o print_distances example/print_distances.cpp
print_mrca: CompactTree/compact_tree.h example/print_mrca.cpp
	$(CXX) $(RELEASEFLAGS) -o print_mrca example/print_mrca.cpp
print_node_data: CompactTree/compact_tree.h example/print_node_data.cpp
	$(CXX) $(RELEASEFLAGS) -o print_node_data example/print_node_data.cpp
print_num_descendants: CompactTree/compact_tree.h example/print_num_descendants.cpp
	$(CXX) $(RELEASEFLAGS) -o print_num_descendants example/print_num_descendants.cpp
print_root_dists: CompactTree/compact_tree.h example/print_root_dists.cpp
	$(CXX) $(RELEASEFLAGS) -o print_root_dists example/print_root_dists.cpp
print_stats: CompactTree/compact_tree.h example/print_stats.cpp
	$(CXX) $(RELEASEFLAGS) -o print_stats example/print_stats.cpp
print_subtree_mrca: CompactTree/compact_tree.h example/print_subtree_mrca.cpp
	$(CXX) $(RELEASEFLAGS) -o print_subtree_mrca example/print_subtree_mrca.cpp
print_topology: CompactTree/compact_tree.h example/print_topology.cpp
	$(CXX) $(RELEASEFLAGS) -o print_topology example/print_topology.cpp
clean:
	$(RM) -r $(EXES) *.o *.so html latex compact_tree.py CompactTree/compact_tree.py *.cxx CompactTree/*.cxx *.gch CompactTree/*.gch CompactTree/*.so __pycache__ *.egg-info build dist

# SWIG
swig: CompactTree/compact_tree.h CompactTree/compact_tree.i
	make clean
	swig -c++ -python CompactTree/compact_tree.i
	g++ -fpic -c CompactTree/compact_tree.h compact_tree_wrap.cxx -I/usr/include/python3.10
	g++ -shared compact_tree_wrap.o -o _compact_tree.so -lstdc++

# Twine
twine: CompactTree/compact_tree.h CompactTree/compact_tree.i
	#swig -c++ -python CompactTree/compact_tree.i
	#sed -i 's/from . import _CompactTree/compact_tree.import _compact_tree/g' compact_tree.py
	#mv compact_tree.py CompactTree/
	python3 setup.py sdist bdist_wheel
	#twine upload dist/*.tar.gz
