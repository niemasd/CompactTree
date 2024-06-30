/**
 * Load a tree
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1]);
    CT_NODE_T last_node = tree.get_num_nodes() - 1;
    compact_tree subtree = tree.extract_tree({last_node, last_node-1});
    return 0;
}
