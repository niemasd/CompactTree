/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H
#include <iostream>
class compact_tree {
    public:
        /**
         * Load a tree from a file
         * @param fn The filename of the tree to load
         * @param schema The schema of `fn`
         */
        compact_tree(const char* const & fn, const char* const & schema) {
            std::cout << "Load " << schema << " file: " << fn << std::endl; // TODO
        }
};
#endif
