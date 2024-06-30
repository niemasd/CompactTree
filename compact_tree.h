/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H

// include statements
#include <cstdint>       // std::uint32_t, std::uint64_t
#include <cstring>       // strcmp()
#include <fcntl.h>       // O_RDONLY, open(), posix_fadvise()
#include <iostream>      // std::cerr, std::cout, std::endl
#include <queue>         // std::queue
#include <stdexcept>     // std::invalid_argument
#include <string>        // std::string
#include <unistd.h>      // read()
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

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

// general constants
#define VERSION "0.0.1"
#define IO_BUFFER_SIZE 16384
const std::string EMPTY_STRING = "";
const CT_NODE_T NULL_NODE = (CT_NODE_T)(-1);

// error messages
const std::string ERROR_OPENING_FILE = "Error opening file";
const std::string ERROR_INVALID_NEWICK_FILE = "Invalid Newick file";
const std::string ERROR_INVALID_NEWICK_STRING = "Invalid Newick string";

// compact_tree class
class compact_tree {
    private:
        /**
         * compact_tree important member variables
         */
        std::vector<CT_NODE_T> parent;                // `parent[i]` is the parent of node `i`
        std::vector<std::vector<CT_NODE_T>> children; // `children[i]` is a `vector` containing the children of node `i`
        std::vector<std::string> label;               // `label[i]` is the label of node `i`
        std::vector<CT_LENGTH_T> length;              // `length[i]` is the length of the edge incident to (i.e., going into) node `i`

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
         * Load a tree from a Newick file or C string
         * @param input The filename or C string of the tree to load
         * @param is_fn `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as a C string
         * @param store_labels `true` to store node labels (default), otherwise `false` (saves memory)
         * @param store_lengths `true` to store edge lengths (default), otherwise `false` (saves memory)
         * @param reserve How many nodes to reserve memory for up-front to avoid `std::vector` resizes. It's fine if the true number of nodes in the tree exceeds this value (the `std::vector` will resize automatically), but get as close as possible for speed.
         */
        compact_tree(char* input, bool is_fn = true, bool store_labels = true, bool store_lengths = true, size_t reserve = 0);

        /**
         * Copy constructor
         * @param `o` The other `compact_tree` to copy
         */
        compact_tree(const compact_tree & o) : parent(o.parent), children(o.children), label(o.label), length(o.length), num_leaves(o.num_leaves) {}

        /**
         * Load a tree from a Newick file or std::string
         * @param input The filename or std::string of the tree to load
         * @param is_fn `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as a std::string
         * @param store_labels `true` to store node labels (default), otherwise `false` (saves memory)
         * @param store_lengths `true` to store edge lengths (default), otherwise `false` (saves memory)
         */

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
        const std::vector<CT_NODE_T> & get_children(CT_NODE_T node) const { return children[node]; }

        /**
         * Get the incident edge length of a node
         * @param node The node to get the incident edge length of
         * @return The incident edge length of `node`
         */
        CT_LENGTH_T get_edge_length(CT_NODE_T node) const { return (length.size() == 0) ? (CT_LENGTH_T)0. : length[node]; }

        /**
         * Set the incident edge length of a node
         * @param node The node to set the incident edge length of
         * @param The new edge length to set
         */
        void set_edge_length(CT_NODE_T node, CT_LENGTH_T new_length) {
            if(length.size() == 0) { length = std::vector<CT_LENGTH_T>(get_num_nodes(), (CT_LENGTH_T)0.); } length[node] = new_length;
        }

        /**
         * Get the label of a node
         * @param node The node to get the label of
         * @return The label of `node`
         */
        const std::string & get_label(CT_NODE_T node) const { return (label.size() == 0) ? EMPTY_STRING : label[node]; }

        /**
         * Set the label of a node
         * @param node The node to set the label of
         * @param new_label The new label to set
         */
        void set_label(CT_NODE_T node, const std::string & new_label) {
            if(label.size() == 0) { label = std::vector<std::string>(get_num_nodes(), ""); } label[node] = new_label;
        }

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

        /**
         * Postorder traversal iterator. The only guarantee is that a node will be visited before its parent
         */
        class postorder_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node;
            public:
                postorder_iterator(CT_NODE_T x) : node(x) {}
                postorder_iterator(const postorder_iterator & it) : node(it.node) {}
                postorder_iterator & operator++() { --node; return *this; }
                postorder_iterator operator++(int) { postorder_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const postorder_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const postorder_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        postorder_iterator postorder_begin() { return postorder_iterator((CT_NODE_T)(get_num_nodes() - 1)); }
        postorder_iterator postorder_end() { return postorder_iterator(NULL_NODE); }

        /**
         * Leaves iterator
         */
        class leaves_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node; const compact_tree* const tree_ptr;
            public:
                leaves_iterator(CT_NODE_T x, const compact_tree* const tp) : node(x), tree_ptr(tp) {}
                leaves_iterator(const leaves_iterator & it) : node(it.node), tree_ptr(it.tree_ptr) {}
                leaves_iterator & operator++() { CT_NODE_T num_nodes = tree_ptr->get_num_nodes(); while(((++node) < num_nodes) && (tree_ptr->children[node].size() != 0)) {} return *this; }
                leaves_iterator operator++(int) { leaves_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const leaves_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const leaves_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        leaves_iterator leaves_begin() { CT_NODE_T node = (CT_NODE_T)0; CT_NODE_T num_nodes = get_num_nodes(); while(((++node) < num_nodes) && (children[node].size() != 0)) {} return leaves_iterator(node, this); }
        leaves_iterator leaves_end() { return leaves_iterator((CT_NODE_T)get_num_nodes(), this); }

        /**
         * Find and return the Most Recent Common Ancestor (MRCA) of a collection of nodes
         * @param nodes The nodes to find the MRCA of
         * @return The MRCA of the nodes in `nodes`
         */
        CT_NODE_T find_mrca(const std::unordered_set<CT_NODE_T> & nodes);

        /**
         * Calculate the total branch length of this tree
         * @param include_internal `true` to include internal nodes, otherwise `false`
         * @param include_leaves `true` to include leaves, otherwise `false`
         * @return The total branch length of this tree
         */
        double calc_total_bl(bool include_internal = true, bool include_leaves = true) {
            if(!(include_internal || include_leaves)) { return 0.; }
            double tot = 0; CT_NODE_T num_nodes = get_num_nodes();
            for(CT_NODE_T node = 0; node < num_nodes; ++node) {
                if(children[node].size() == 0) { if(include_leaves) { tot += get_edge_length(node); } }
                else { if(include_internal) { tot += get_edge_length(node); } }
            }
            return tot;
        }

        /**
         * Calculate the average branch length of this tree
         * @param include_internal `true` to include internal nodes, otherwise `false`
         * @param include_leaves `true` to include leaves, otherwise `false`
         * @return The average branch length of this tree
         */
        double calc_avg_bl(bool include_internal = true, bool include_leaves = true) {
            if(!(include_internal || include_leaves)) { return 0.; }
            double tot = calc_total_bl(include_internal, include_leaves); CT_NODE_T den = 0;
            if(include_internal) { den += get_num_internal(); }
            if(include_leaves) { den += get_num_leaves(); }
            return tot / den;
        }

        /**
         * Calculate the (weighted) distance between two nodes
         * @param `u` The first node
         * @param `v` The second node
         * @return The (weighted) distance between `u` and `v`
         */
        double calc_dist(CT_NODE_T u, CT_NODE_T v) {
            if(u == v) { return 0.; } if(u == parent[v]) { return get_edge_length(v); } if(v == parent[u]) { return get_edge_length(u); }
            std::unordered_map<CT_NODE_T, double> u_dists; std::unordered_map<CT_NODE_T, double> v_dists; CT_NODE_T c = u; CT_NODE_T p = parent[c]; std::unordered_map<CT_NODE_T, double>::iterator it;
            while(p != NULL_NODE) { u_dists[p] = u_dists[c] + get_edge_length(c); c = p; p = parent[p]; } c = v; p = parent[c];
            while(p != NULL_NODE) { v_dists[p] = v_dists[c] + get_edge_length(c); it = u_dists.find(p); if(it != u_dists.end()) { return it->second + v_dists[p]; } c = p; p = parent[p]; }
            return -1.; // error, shouldn't reach here
        }
};

// find the MRCA of nodes
CT_NODE_T compact_tree::find_mrca(const std::unordered_set<CT_NODE_T> & nodes) {
    std::queue<CT_NODE_T, std::deque<CT_NODE_T>> to_visit(std::deque<CT_NODE_T>(nodes.begin(), nodes.end()));
    std::unordered_map<CT_NODE_T, CT_NODE_T> count; std::unordered_map<CT_NODE_T, CT_NODE_T>::iterator count_it;
    CT_NODE_T curr_node; CT_NODE_T curr_parent; size_t total = nodes.size();
    while(!to_visit.empty()) {
        curr_node = to_visit.front(); to_visit.pop(); count_it = count.find(curr_node); curr_parent = parent[curr_node];
        if(count_it == count.end()) { // current node hasn't been seen before, initialize its count to 0
            count_it = count.emplace(curr_node, 0).first;
        }
        if((++(count_it->second)) == total) {
            return curr_node;
        }
        if(curr_parent != NULL_NODE) {
            to_visit.emplace(curr_parent);
        }
    }
    return NULL_NODE; // shouldn't ever reach here
}

// helper function to create new node and add as child to parent
CT_NODE_T compact_tree::create_child(const CT_NODE_T parent_node) {
    tmp_node = parent.size();                        // `tmp_node` = new child node
    parent.emplace_back(parent_node);                // `parent[tmp_node]` = parent of new node (which is `parent_node`)
    children.emplace_back(std::vector<CT_NODE_T>()); // `children[tmp_node]` = children of new node (currently empty)
    if(length.size() != 0) {
        length.emplace_back((CT_LENGTH_T)0);         // `length[tmp_node]` = incident edge length of new node (currently 0)
    }
    if(label.size() != 0) {
        label.emplace_back("");                      // `label[tmp_node]` = label of new node (currently nothing)
    }
    children[parent_node].emplace_back(tmp_node);    // add `tmp_node` as a new child of `parent_node`
    return tmp_node;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(char* input, bool is_fn, bool store_labels, bool store_lengths, size_t reserve) {
    // reserve space up-front (if given `reserve`) to reduce resizing (save time)
    if(reserve != 0) { parent.reserve(reserve); if(store_lengths) { length.reserve(reserve); } if(store_labels) { label.reserve(reserve); } }

    // set up file input: https://stackoverflow.com/a/17925143/2134991
    int fd = -1;
    size_t bytes_read = 0; size_t i;              // variables to help with reading
    char read_buf[IO_BUFFER_SIZE + 1];            // buffer for reading
    char str_buf[256] = {}; size_t str_buf_i = 0; // helper string buffer
    char* buf;                                    // either read_buf (if reading from file) or the C string (if reading Newick string)
    if(is_fn) {
        fd = open(input, O_RDONLY);
        if(fd == -1) {
            throw std::invalid_argument(ERROR_OPENING_FILE + ": " + input);
        }
        posix_fadvise(fd, 0, 0, 1);               // FDADVICE_SEQUENTIAL
        buf = read_buf;
    } else {
        bytes_read = strlen(input);
        buf = input;
    }

    // set up root node (initially empty/blank)
    parent.emplace_back(NULL_NODE);
    children.emplace_back(std::vector<CT_NODE_T>());
    if(store_lengths) { length.emplace_back((CT_LENGTH_T)0); }
    if(store_labels) { label.emplace_back(""); }

    // set up initial Newick parsing state
    CT_NODE_T curr_node = 0;    // start at root node (0)
    bool parse_length = false;  // parsing a length right now?
    bool parse_label = false;   // parsing a label right now?
    bool parse_comment = false; // parsing a comment [...] right now?

    // read Newick tree byte-by-byte
    while(true) {
        // either read from file or from Newick C string
        if(is_fn) {
            bytes_read = read(fd, buf, IO_BUFFER_SIZE);
        } else {
            buf = input;
        }

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
                        if(length.size() != 0) {
                            length[curr_node] = atof(str_buf);
                        }
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
                        if(label.size() != 0) {
                            label[curr_node] = str_buf;
                        }
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
                        if(curr_node == NULL_NODE) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
                        }
                        curr_node = create_child(curr_node);
                        break;

                    // go to parent
                    case ')':
                        curr_node = parent[curr_node];
                        break;

                    // go to new sibling
                    case ',':
                        if((curr_node == NULL_NODE) || (parent[curr_node] == NULL_NODE)) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
                        }
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
    throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
}
#endif
