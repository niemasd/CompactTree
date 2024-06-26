/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H

// include statements
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <unordered_map>
#include <vector>

// constants
#define CT_NODE_T std::uint32_t // define node type, which is a fixed-width unsigned integer (supports up to 4 billion nodes)

// compact_tree class
class compact_tree {
    private:
        /**
         * compact_tree member variables
         */
        std::vector<CT_NODE_T> parent;                            // `parent[i]` is the parent of node `i`
        std::vector<std::vector<CT_NODE_T>> children;             // `children[i]` is a `vector` containing the children of node `i`
        std::vector<const char*> label;                           // `label[i]` is the label of node `i`
        std::unordered_map<const char*, CT_NODE_T> label_to_node; // `label_to_node[label]` is the node labeled by `label`

    public:
        /**
         * Load a tree from a file
         * @param fn The filename of the tree to load
         * @param schema The schema of `fn`
         */
        compact_tree(const char* const & fn, char* const & schema);

        /**
         * Get the label of a node
         * @param node The node to get the label of
         * @return The label of `node`
         */
        const char* const get_label(CT_NODE_T node);

        /**
         * Get the label associated with a node
         * @param label The label
         * @return The node labeled by `label`, or -1 if not found
         */
        CT_NODE_T get_node(const char* const & label);
};

// compact_tree constructor
compact_tree::compact_tree(const char* const & fn, char* const & schema) {
    // convert schema to lowercase
    for(uint8_t i = 0; schema[i]; ++i) {
        schema[i] = tolower(schema[i]);
    }

    // load Newick tree
    if(strcmp(schema, "newick") == 0) {
        std::cout << "Load " << schema << " file: " << fn << std::endl; // TODO
    }

    // invalid schema
    else {
        std::cerr << "ERROR: Invalid schema: " << schema << std::endl; exit(1);
    }
}

// get label from node
const char* const compact_tree::get_label(CT_NODE_T node) {
    return label[node];
}

// get node from label
CT_NODE_T compact_tree::get_node(const char* const & label) {
    auto tmp = label_to_node.find(label);
    if(tmp == label_to_node.end()) {
        return (CT_NODE_T)(-1);
    }
    return tmp->second;
}
#endif
