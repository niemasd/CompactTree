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
    compact_tree tree(argv[1]);

    // print stats
    std::cout << "Number of Nodes: " << tree.get_num_nodes() << std::endl;
    std::cout << "- Leaves: " << tree.get_num_leaves() << std::endl;
    std::cout << "- Internal: " << tree.get_num_internal() << std::endl;
    std::cout << "Total Branch Length: " << tree.calc_total_bl() << std::endl;
    std::cout << "- Leaves: " << tree.calc_total_bl(false, true) << std::endl;
    std::cout << "- Internal: " << tree.calc_total_bl(true, false) << std::endl;
    std::cout << "Average Branch Length: " << tree.calc_avg_bl() << std::endl;
    std::cout << "- Leaves: " << tree.calc_avg_bl(false, true) << std::endl;
    std::cout << "- Internal: " << tree.calc_avg_bl(true, false) << std::endl;
    return 0;
}
