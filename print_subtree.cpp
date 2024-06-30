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
    // TODO pick the last 2 leaves and extract their subtree to print to std::cout
    return 0;
}
