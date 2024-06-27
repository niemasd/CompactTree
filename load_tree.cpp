/**
 * Load a tree
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file> <schema>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], argv[2]);
}
