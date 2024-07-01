/**
 * Load a tree
 */
#include <chrono>
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    auto start = std::chrono::system_clock::now();
    compact_tree tree(argv[1]);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    //std::cout << "Loaded tree with " << tree.get_num_nodes() << " nodes in " << elapsed.count() << " seconds" << std::endl;
    tree.print_newick(std::cout); std::cout << std::endl; // TODO DELETE
    return 0;
}
