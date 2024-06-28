/**
 * Print root distances of all labeled nodes in the tree to showcase preorder traversal
 */
#include <iostream>
#include <vector>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file> <schema>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], argv[2]);

    // calculate and print root distances
    std::vector<CT_LENGTH_T> root_dists(tree.get_num_nodes(), (CT_LENGTH_T)0);
    compact_tree::preorder_iterator it_end = tree.preorder_end();
    CT_NODE_T curr_node; CT_NODE_T root = tree.get_root();
    const std::string* curr_label_ptr;
    for(compact_tree::preorder_iterator it = tree.preorder_begin(); it != it_end; ++it) {
        curr_node = *it; curr_label_ptr = &tree.get_label(curr_node);
        if(curr_node != root) {
            root_dists[curr_node] = root_dists[tree.get_parent(curr_node)] + tree.get_edge_length(curr_node);
        }
        if((*curr_label_ptr).size() != 0) {
            std::cout << (*curr_label_ptr) << ": " << root_dists[curr_node] << std::endl;
        }
    }
    return 0;
}
