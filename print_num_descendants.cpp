/**
 * Print the number of descendants (including the node itself) of all labeled nodes in the tree to showcase postorder traversal
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
    std::vector<unsigned int> num_descendants(tree.get_num_nodes(), 1);
    compact_tree::postorder_iterator it_end = tree.postorder_end();
    CT_NODE_T curr_node;
    for(compact_tree::postorder_iterator it = tree.postorder_begin(); it != it_end; ++it) {
        curr_node = *it;
        const std::vector<CT_NODE_T> & curr_children = tree.get_children(curr_node);
        if(curr_children.size() != 0) {
            for(auto child_node : curr_children) {
                num_descendants[curr_node] += num_descendants[child_node];
            }
        }
        std::cout << tree.get_label(curr_node) << ": " << num_descendants[curr_node] << std::endl;
    }
    return 0;
}
