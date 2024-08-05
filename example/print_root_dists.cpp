/**
 * Print root distances of all labeled nodes in the tree to showcase pre-order and level-order traversal
 */
#include <iostream>
#include <vector>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1]);

    // set things up
    std::vector<CT_LENGTH_T> root_dists_weighted(tree.get_num_nodes(), (CT_LENGTH_T)0);
    std::vector<uint32_t> root_dists_unweighted(tree.get_num_nodes(), 0);
    CT_NODE_T curr_node; CT_NODE_T root = tree.get_root();
    const std::string* curr_label_ptr;

    // calculate weighted root distances using pre-order traversal
    std::cout << "=== Weighted Root Distances ===" << std::endl;
    compact_tree::preorder_iterator it_end_pre = tree.preorder_end();
    for(compact_tree::preorder_iterator it = tree.preorder_begin(); it != it_end_pre; ++it) {
        curr_node = *it; curr_label_ptr = &tree.get_label(curr_node);
        if(curr_node != root) {
            root_dists_weighted[curr_node] = root_dists_weighted[tree.get_parent(curr_node)] + tree.get_edge_length(curr_node);
        }
        if((*curr_label_ptr).size() != 0) {
            std::cout << (*curr_label_ptr) << ": " << root_dists_weighted[curr_node] << std::endl;
        }
    }

    // calculate unweighted root distances using level-order traversal
    std::cout << "=== Unweighted Root Distances ===" << std::endl;
    compact_tree::levelorder_iterator it_end_level = tree.levelorder_end();
    for(compact_tree::levelorder_iterator it = tree.levelorder_begin(); it != it_end_level; ++it) {
        curr_node = *it; curr_label_ptr = &tree.get_label(curr_node);
        if(curr_node != root) {
            root_dists_unweighted[curr_node] = root_dists_unweighted[tree.get_parent(curr_node)] + 1;
        }
        if((*curr_label_ptr).size() != 0) {
            std::cout << (*curr_label_ptr) << ": " << root_dists_unweighted[curr_node] << std::endl;
        }
    }
    return 0;
}
