/**
 * Print root distances of all labeled nodes in the tree to showcase pre-order and level-order traversal
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
    std::tuple<const std::string*, CT_LENGTH_T, CT_NODE_T, const std::vector<CT_NODE_T>*> curr_data;
    const std::string* curr_label_ptr;
    CT_LENGTH_T curr_length;
    CT_NODE_T curr_parent;
    const std::vector<CT_NODE_T>* curr_children_ptr;
    size_t curr_children_size;
    size_t curr_child_ind;
    compact_tree::preorder_iterator it_end = tree.preorder_end();
    for(compact_tree::preorder_iterator it = tree.preorder_begin(); it != it_end; ++it) {
        curr_node = *it;
        curr_data = tree[curr_node];
        curr_label_ptr = std::get<0>(curr_data);
        curr_length = std::get<1>(curr_data);
        curr_parent = std::get<2>(curr_data);
        curr_children_ptr = std::get<3>(curr_data);
        curr_children_size = curr_children_ptr->size();
        std::cout << "- Node " << curr_node << std::endl;
        if(!(curr_label_ptr->empty())) {
            std::cout << "  - Label: " << (*curr_label_ptr) << std::endl;
        }
        std::cout << "  - Edge Length: " << curr_length << std::endl;
        if(curr_parent != NULL_NODE) {
            std::cout << "  - Parent: Node " << curr_parent << std::endl;
        }
        if(curr_children_size != 0) {
            std::cout << "  - Children: {Node " << (*curr_children_ptr)[0];
            for(curr_child_ind = 1; curr_child_ind < curr_children_size; ++curr_child_ind) {
                std::cout << ", Node " << (*curr_children_ptr)[curr_child_ind];
            }
            std::cout << '}' << std::endl;
        }
    }
    return 0;
}
