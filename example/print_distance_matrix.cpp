/**
 * Print all pair distances of leaves in the tree to showcase leaf iteration and pairwise distance calculation
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

    // calculate and print the pairwise distance matrix
    std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> dm = tree.calc_distance_matrix();
    std::cout << "u\tv\td" << std::endl;
    for(std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double> curr : dm) {
        std::cout << tree.get_label(curr.first.first) << '\t' << tree.get_label(curr.first.second) << '\t' << curr.second << std::endl;
    }
    return 0;
}
