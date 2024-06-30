/**
 * Load a tree topology (no labels or lengths)
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], true, false, false);
    tree.print_newick(std::cout);
    std::cout << std::endl;
    return 0;
}
