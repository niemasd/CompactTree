/**
 * Print summary data about every node in the tree
 */
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1]);

    // print every node's data
    CT_NODE_T curr_node;
    std::tuple<const std::string*, CT_LENGTH_T> curr_data;
    const std::string* curr_label_ptr;
    CT_LENGTH_T curr_length;
    compact_tree::preorder_iterator it_end = tree.preorder_end();
    for(compact_tree::preorder_iterator it = tree.preorder_begin(); it != it_end; ++it) {
        curr_node = *it;
        curr_data = tree[curr_node];
        curr_label_ptr = std::get<0>(curr_data);
        curr_length = std::get<1>(curr_data);
        std::cout << "- Node " << curr_node << std::endl;
        if(!(curr_label_ptr->empty())) {
            std::cout << "  - Label: " << (*curr_label_ptr) << std::endl;
        }
        std::cout << "  - Edge Length: " << curr_length << std::endl;
    }
    return 0;
}
