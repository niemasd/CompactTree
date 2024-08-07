/**
 * Print all pairwise distances of leaves in the tree to showcase leaf iteration and pairwise distance calculation
 */
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1]);

    // find and print all pairwise leaf distances
    std::vector<CT_NODE_T> leaves(tree.leaves_begin(), tree.leaves_end()); size_t num_leaves = leaves.size();
    std::cout << "u\tv\td" << std::endl;
    for(size_t i = 0; i < num_leaves-1; ++i) {
        CT_NODE_T u = leaves[i]; const std::string & ul = tree.get_label(u);
        for(size_t j = i+1; j < num_leaves; ++j) {
            CT_NODE_T v = leaves[j]; const std::string & vl = tree.get_label(v);
            std::cout << ul << '\t' << vl << '\t' << tree.calc_dist(u,v) << std::endl;
        }
    }
    return 0;
}
