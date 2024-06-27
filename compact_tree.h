/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H

// include statements
#include <cstring>       // strcmp()
#include <fcntl.h>       // O_RDONLY, open(), posix_fadvise()
#include <iostream>      // std::cerr, std::cout, std::endl
#include <unistd.h>      // read()
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

// general constants
#define VERSION "0.0.1"
#define IO_BUFFER_SIZE 16384

// define node type, which is a fixed-width unsigned integer (default is 32-bit)
#if defined CT_NODE_64
#define CT_NODE_T std::uint64_t
#else
#define CT_NODE_T std::uint32_t
#endif

// define edge length type, which is a floating point number (default is float)
#if defined CT_LENGTH_DOUBLE
#define CT_LENGTH_T double
#else
#define CT_LENGTH_T float
#endif

// compact_tree class
class compact_tree {
    private:
        /**
         * compact_tree member variables
         */
        std::vector<CT_NODE_T> parent;                            // `parent[i]` is the parent of node `i`
        std::vector<std::vector<CT_NODE_T>> children;             // `children[i]` is a `vector` containing the children of node `i`
        std::vector<CT_LENGTH_T> length;                          // `length[i]` is the length of the edge incident to (i.e., going into) node `i`
        std::vector<const char*> label;                           // `label[i]` is the label of node `i`
        std::unordered_map<const char*, CT_NODE_T> label_to_node; // `label_to_node[label]` is the node labeled by `label`

    public:
        /**
         * Load a tree from a file
         * @param fn The filename of the tree to load
         * @param schema The schema of `fn`
         */
        compact_tree(const char* const fn, char* const schema);

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
        CT_NODE_T get_node(const char* const label);
};

// compact_tree constructor
compact_tree::compact_tree(const char* const fn, char* const schema) {
    // convert schema to lowercase
    for(size_t i = 0; schema[i]; ++i) {
        schema[i] = tolower(schema[i]);
    }

    // load Newick tree
    if(strcmp(schema, "newick") == 0) {
        // set up file input: https://stackoverflow.com/a/17925143/2134991
        int fd = open(fn, O_RDONLY);
        if(fd == -1) {
            return; // error opening file
        }
        posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL
        char buf[IO_BUFFER_SIZE + 1];

        // read input byte-by-byte
        size_t bytes_read; char* p; size_t i;
        while((bytes_read = read(fd, buf, IO_BUFFER_SIZE))) {
            if(bytes_read == (size_t)(-1)) {
                return; // read failed
            }
            if(!bytes_read) {
                break;
            }
            for(p = buf, i = 0; i < bytes_read; ++p, ++i) {
                switch(*p) {
                    // end of Newick string
                    case ';':
                        // TODO
                        return;

                    // go to new child
                    case '(':
                        // TODO
                        break;

                    // go to parent
                    case ')':
                        // TODO
                        break;

                    // go to new sibling
                    case ',':
                        // TODO
                        break;

                    // edge length
                    case ':':
                        // TODO
                        break;

                    // node label
                    default:
                        // TODO
                        break; // unnecessary (can delete once I fill in this switch case
                }
            }
        }
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
CT_NODE_T compact_tree::get_node(const char* const label) {
    auto tmp = label_to_node.find(label);
    if(tmp == label_to_node.end()) {
        return (CT_NODE_T)(-1);
    }
    return tmp->second;
}
#endif
