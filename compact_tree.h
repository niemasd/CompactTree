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
#include <string>        // std::string
#include <unistd.h>      // read()
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
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
         * compact_tree important member variables
         */
        std::vector<CT_NODE_T> parent;                // `parent[i]` is the parent of node `i`
        std::vector<std::vector<CT_NODE_T>> children; // `children[i]` is a `vector` containing the children of node `i`
        std::vector<CT_LENGTH_T> length;              // `length[i]` is the length of the edge incident to (i.e., going into) node `i`
        std::vector<std::string> label;               // `label[i]` is the label of node `i`

        /**
         * compact_tree helper member variables
         */
        CT_NODE_T tmp_node;      // temporary holding variable for nodes (e.g. new child); value should only be used immediately after assigning
        size_t num_leaves = 0;   // cache the total number of leaves (avoid recalculation)

        /**
         * Helper function to create a new node and add it as a child to a given node
         * @param parent_node The parent node
         * @return The newly-created child node
         */
        CT_NODE_T create_child(const CT_NODE_T parent_node);

        /**
         * Helper function to calculate the number of leaves in O(n), which can then be used to calculate the number of internal nodes in O(1)
         */
        void calc_num_leaves() { for(const auto & curr_children : children) { if(curr_children.size() == 0) { num_leaves += 1; } } }

    public:
        /**
         * Load a tree from a file
         * @param fn The filename of the tree to load
         * @param schema The schema of `fn`
         */
        compact_tree(const char* const fn, char* const schema);

        /**
         * Get the total number of nodes in the tree using a O(1) lookup
         * @return The number of nodes in the tree
         */
        size_t get_num_nodes() const { return parent.size(); }

        /**
         * Get the total number of leaves in the tree. First time is O(n) scan; subsequent times are O(1) lookup
         * @return The number of leaves in the tree
         */
        size_t get_num_leaves() { if(num_leaves == 0) { calc_num_leaves(); } return num_leaves; }

        /**
         * Get the total number of internal nodes in the tree. First time is O(n) scan; subsequent times are O(1) lookup
         * @return The number of internal nodes in the tree
         */
        size_t get_num_internal() { return get_num_nodes() - get_num_leaves(); }

        /**
         * Get the root of this tree (should always be 0)
         * @return The root of this tree
         */
        CT_NODE_T get_root() const { return (CT_NODE_T)0; }

        /**
         * Get the parent of a node
         * @param node The node to get the parent of
         * @return The parent of `node`
         */
        CT_NODE_T get_parent(CT_NODE_T node) const { return parent[node]; }

        /**
         * Get the children of a node
         * @param node The node to get the children of
         * @return The children of `node`
         */
        const std::vector<CT_NODE_T> get_children(CT_NODE_T node) const { return children[node]; }

        /**
         * Get the incident edge length of a node
         * @param node The node to get the incident edge length of
         * @return The incident edge length of `node`
         */
        CT_LENGTH_T get_edge_length(CT_NODE_T node) const { return length[node]; }

        /**
         * Get the label of a node
         * @param node The node to get the label of
         * @return The label of `node`
         */
        const std::string & get_label(CT_NODE_T node) const { return label[node]; }

        /**
         * Get all labels (return by reference)
         * @return A `vector<string>` where the `i`-th value is the label of node `i`
         */
        const std::vector<std::string> & get_labels() const { return label; }

        /**
         * Preorder traversal iterator. The only guarantee is that a node will be visited before its children
         */
        class preorder_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node;
            public:
                preorder_iterator(CT_NODE_T x) : node(x) {}
                preorder_iterator(const preorder_iterator & it) : node(it.node) {}
                preorder_iterator & operator++() { ++node; return *this; }
                preorder_iterator operator++(int) { preorder_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const preorder_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const preorder_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        preorder_iterator preorder_begin() { return preorder_iterator((CT_NODE_T)0); }
        preorder_iterator preorder_end() { return preorder_iterator((CT_NODE_T)get_num_nodes()); }
};

// helper function to create new node and add as child to parent
CT_NODE_T compact_tree::create_child(const CT_NODE_T parent_node) {
    tmp_node = parent.size();                        // `tmp_node` = new child node
    parent.emplace_back(parent_node);                // `parent[tmp_node]` = parent of new node (which is `parent_node`)
    children.emplace_back(std::vector<CT_NODE_T>()); // `children[tmp_node]` = children of new node (currently empty)
    length.emplace_back((CT_LENGTH_T)0);             // `length[tmp_node]` = incident edge length of new node (currently 0)
    label.emplace_back("");                          // `label[tmp_node]` = label of new node (currently nothing)
    children[parent_node].emplace_back(tmp_node);    // add `tmp_node` as a new child of `parent_node`
    return tmp_node;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(const char* const fn, char* const schema) {
    // convert schema to lowercase
    for(size_t i = 0; schema[i]; ++i) {
        schema[i] = tolower(schema[i]);
    }

    // set up root node (initially empty/blank)
    parent.emplace_back((CT_NODE_T)(-1));
    children.emplace_back(std::vector<CT_NODE_T>());
    length.emplace_back((CT_LENGTH_T)0);
    label.emplace_back("");

    // load Newick tree
    if(strcmp(schema, "newick") == 0) {
        // set up file input: https://stackoverflow.com/a/17925143/2134991
        int fd = open(fn, O_RDONLY);
        if(fd == -1) {
            return; // error opening file
        }
        posix_fadvise(fd, 0, 0, 1);                   // FDADVICE_SEQUENTIAL
        char buf[IO_BUFFER_SIZE + 1];                 // buffer for reading
        size_t bytes_read; size_t i;                  // variables to help with reading
        char str_buf[256] = {}; size_t str_buf_i = 0; // helper string buffer

        // set up initial Newick parsing state
        CT_NODE_T curr_node = 0;    // start at root node (0)
        bool parse_length = false;  // parsing a length right now?
        bool parse_label = false;   // parsing a label right now?
        bool parse_comment = false; // parsing a comment [...] right now?

        // read Newick tree byte-by-byte
        while((bytes_read = read(fd, buf, IO_BUFFER_SIZE))) {
            // handle cases where we don't use the values in the buffer
            if(bytes_read == (size_t)(-1)) {
                return; // read failed
            }
            if(!bytes_read) {
                break; // done reading
            }

            // iterate over the characters in the buffer
            for(i = 0; i < bytes_read; ++i) {
                // currently parsing a comment (ignore for now)
                if(parse_comment) {
                    // reached end of comment
                    if(buf[i] == ']') {
                        parse_comment = false;
                    }
                }

                // currently parsing an edge length
                else if(parse_length) {
                    switch(buf[i]) {
                        // finished parsing edge length
                        case ',':
                        case ')':
                        case ';':
                            str_buf[str_buf_i] = (char)0;
                            parse_length = false;
                            --i; // need to re-read this character
                            length[curr_node] = atof(str_buf);
                            break;

                        // edge comment (ignore for now)
                        case '[':
                            parse_comment = true;
                            break;

                        // character within edge length
                        default:
                            str_buf[str_buf_i++] = buf[i];
                            break;
                    }
                }

                // currently parsing a node label
                else if(parse_label) {
                    switch(buf[i]) {
                        // finished parsing node label
                        case ':':
                        case ',':
                        case ')':
                            --i; // need to re-read this character
                        case '\'':
                            str_buf[str_buf_i] = (char)0;
                            parse_label = false;
                            label[curr_node] = str_buf;
                            break;

                        // node comment (ignore for now)
                        case '[':
                            parse_comment = true;
                            break;

                        // character within node label
                        default:
                            str_buf[str_buf_i++] = buf[i];
                            break;
                    }
                }

                // all other symbols
                else {
                    switch(buf[i]) {
                        // ignore spaces outside of labels
                        case ' ':
                            break;

                        // end of Newick string
                        case ';':
                            return;

                        // go to new child
                        case '(':
                            curr_node = create_child(curr_node);
                            break;

                        // go to parent
                        case ')':
                            curr_node = parent[curr_node];
                            break;

                        // go to new sibling
                        case ',':
                            curr_node = create_child(parent[curr_node]);
                            break;

                        // node comment (ignore for now)
                        case '[':
                            parse_comment = true;
                            break;

                        // edge length
                        case ':':
                            parse_length = true; str_buf_i = 0;
                            break;

                        // about to parse a node label in quotes ('')
                        case '\'':
                            parse_label = true; str_buf_i = 0;

                        // about to start a node label without quotes
                        default:
                            parse_label = true; str_buf_i = 0;
                            --i; // need to re-read this character (it's part of the label)
                            break;
                    }
                }
            }
        }
    }

    // invalid schema
    else {
        std::cerr << "ERROR: Invalid schema: " << schema << std::endl; exit(1);
    }
}
#endif
