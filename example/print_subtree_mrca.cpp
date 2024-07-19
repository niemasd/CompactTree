/**
 * Print the subtree rooted at the MRCA of two nodes in a given tree
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1]);
    std::cout << "Original:" << std::endl;
    tree.print_newick(std::cout);
    CT_NODE_T last_node = tree.get_num_nodes() - 1;
    compact_tree subtree = tree.extract_subtree(tree.find_mrca({last_node, last_node-1}));
    std::cout << std::endl << std::endl << "Subtree:" << std::endl;
    subtree.print_newick(std::cout);
    std::cout << std::endl;
    return 0;
}
