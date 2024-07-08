/**
 * Load a tree from a Newick string
 */
#include <chrono>
#include <fstream>
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }

    // load file into string
    std::ifstream infile(argv[1]);
    std::string tree_string;
    getline(infile, tree_string);

    // load compact_tree
    auto start = std::chrono::system_clock::now();
    compact_tree tree(tree_string, false);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Loaded tree with " << tree.get_num_nodes() << " nodes in " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
