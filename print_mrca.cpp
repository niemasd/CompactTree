/**
 * Print the MRCAs of all pairs of leaves in the tree to showcase leaf iteration and MRCA finding
 */
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file> <schema>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], argv[2], true, false);

    // find and print all pairwise leaf MRCAs
    std::vector<CT_NODE_T> leaves(tree.leaves_begin(), tree.leaves_end()); size_t num_leaves = leaves.size();
    for(size_t i = 0; i < num_leaves-1; ++i) {
        CT_NODE_T u = leaves[i]; const std::string & ul = tree.get_label(u);
        for(size_t j = i+1; j < num_leaves; ++j) {
            CT_NODE_T v = leaves[j]; const std::string & vl = tree.get_label(v);
            std::cout << ul << ", " << vl << ": " << tree.get_label(tree.find_mrca({u,v})) << std::endl;
        }
    }
    return 0;
}
