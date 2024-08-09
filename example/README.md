# Example Programs

* **[`load_tree.cpp`](load_tree.cpp)** - Load a tree from a file
* **[`load_tree_string.cpp`](load_tree_string.cpp)** - Load a tree from a Newick string
* **[`copy_tree.cpp`](copy_tree.cpp)** - Copy a tree (showcases copy constructor)
* **[`print_topology.cpp`](print_topology.cpp)** - Print the topology of a tree without labels/lengths (showcases memory-efficient way to load a tree by ignoring labels/lengths)
* **[`print_stats.cpp`](print_stats.cpp)** - Print statistics about a tree
* **[`print_node_data.cpp`](print_node_data.cpp)** - Print summary data about every node in the tree
* **[`print_root_dists.cpp`](print_root_dists.cpp)** - Print the weighted and unweighted root distances of the labeled nodes in the tree (showcases pre-order and level-order traversals)
* **[`print_num_descendants.cpp`](print_num_descendants.cpp)** - Print the number of descendants of each labeled node in the tree (showcases postorder traversal)
* **[`print_distances.cpp`](print_distances.cpp)** - Print all leaf pairwise distances (showcases individual pairwise distance calculation)
    * This calculates each *individual* pairwise distance one-by-one to showcase that specific functionality
* **[`print_distance_matrix.cpp`](print_distance_matrix.cpp)** - Print all pairwise distances more efficiently
    * This calculates *all* pairwise distances in a single post-order traversal
* **[`print_mrca.cpp`](print_mrca.cpp)** - Print the Most Recent Common Ancestor (MRCA) of two nodes in the tree (showcases finding MRCA)
* **[`print_subtree_mrca.cpp`](print_subtree_mrca.cpp)** - Extract and print the subtree rooted at the MRCA of two nodes in the tree (showcases subtree extraction)
* **[`python_wrapper.py`](python_wrapper.py)** - Example script using the CompactTree Python wrapper
