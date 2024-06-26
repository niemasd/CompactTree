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
#include <vector>

// constants
#define NODE_T std::uint32_t // define node type, which is a fixed-width unsigned integer

// compact_tree class
class compact_tree {
    private:
        /**
         * compact_tree member variables
         */
        std::vector<NODE_T> parent;                // `parent[i]` is the parent of node `i`
        std::vector<std::vector<NODE_T>> children; // `children[i]` is a `vector` containing the children of node `i`

    public:
        /**
         * Load a tree from a file
         * @param fn The filename of the tree to load
         * @param schema The schema of `fn`
         */
        compact_tree(const char* const & fn, char* const & schema);
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
#endif
