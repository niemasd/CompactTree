/**
 * Print statistics about a tree
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], argv[2], false, true);

    // print stats
    std::cout << "Number of Nodes: " << tree.get_num_nodes() << std::endl;
    std::cout << "- Leaves: " << tree.get_num_leaves() << std::endl;
    std::cout << "- Internal: " << tree.get_num_internal() << std::endl;
    return 0;
}
