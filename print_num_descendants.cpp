/**
 * Print the number of descendants (including the node itself) of all labeled nodes in the tree to showcase postorder traversal
 */
#include <iostream>
#include <vector>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file>" << std::endl; exit(1);
    }
    compact_tree tree(argv[1], true, true, false);

    // calculate and print number of descendants
    std::vector<unsigned int> num_descendants(tree.get_num_nodes(), 1);
    compact_tree::postorder_iterator it_end = tree.postorder_end();
    CT_NODE_T curr_node;
    for(compact_tree::postorder_iterator it = tree.postorder_begin(); it != it_end; ++it) {
        curr_node = *it; auto child_it_end = tree.children_end();
        for(auto child_it = tree.children_begin(curr_node); child_it != child_it_end; ++child_it) {
            num_descendants[curr_node] += num_descendants[*child_it];
        }
        std::cout << tree.get_label(curr_node) << ": " << num_descendants[curr_node] << std::endl;
    }
    return 0;
}
