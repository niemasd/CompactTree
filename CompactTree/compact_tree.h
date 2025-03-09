/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H

// include statements
#include <cstdint>       // std::uint32_t, std::uint64_t
#include <cstdlib>       // std::abs, std::atof
#include <cstring>       // strcmp()
#include <fcntl.h>       // O_RDONLY, open(), posix_fadvise()
#include <iostream>      // std::ostream
#include <limits>        // std::numeric_limits<T>::max
#include <queue>         // std::queue
#include <sstream>       // std::ostringstream
#include <stack>         // std::stack
#include <stdexcept>     // std::invalid_argument
#include <string>        // std::string
#include <tuple>         // std::tuple
#include <unistd.h>      // read()
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::make_pair, std::pair
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
#define PARSE_LENGTH_FROM_C_STR std::atof
#define PARSE_LENGTH_FROM_STRING std::stod
#else
#define CT_LENGTH_T float
#define PARSE_LENGTH_FROM_C_STR (float)std::atof
#define PARSE_LENGTH_FROM_STRING std::stof
#endif

// general constants
#define COMPACTTREE_VERSION "1.0.0"
#define IO_BUFFER_SIZE 16384
#define STR_BUFFER_SIZE 16384
const std::string EMPTY_STRING = "";
const CT_NODE_T ROOT_NODE = (CT_NODE_T)0;
const CT_NODE_T NULL_NODE = std::numeric_limits<CT_NODE_T>::max();
const CT_LENGTH_T ZERO_LENGTH = (CT_LENGTH_T)0.;
const CT_LENGTH_T ZERO_THRESH = (CT_LENGTH_T)0.000001;

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
        size_t num_leaves = 0;   // cache the total number of leaves (avoid recalculation)
        CT_NODE_T tmp_node;      // temporary holding variable for nodes (e.g. new child); value should only be used immediately after assigning

        /**
         * Helper constructor to create a completely empty compact_tree
         */
        compact_tree() {}

        /**
         * Helper function to create a new node and add it as a child to a given node
         * @param parent_node The parent node
         * @param store_labels `true` if this `compact_tree` is storing labels, otherwise `false`
         * @param store-lengths `true` if this `compact_tree` is storing lengths, otherwise `false`
         * @return The newly-created child node
         */
        CT_NODE_T create_child(const CT_NODE_T parent_node, bool store_labels, bool store_lengths);

        /**
         * Helper function to calculate the number of leaves in O(n), which can then be used to calculate the number of internal nodes in O(1)
         */
        void calc_num_leaves() { CT_NODE_T num_nodes = parent.size(); for(CT_NODE_T curr = 0; curr < num_nodes; ++curr) { if(is_leaf(curr)) { num_leaves += 1; } } }

    public:
        /**
         * Load a tree from a Newick file or C string
         * @param input The filename or C string of the tree to load
         * @param is_fn `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as a C string
         * @param store_labels `true` to store node labels (default), otherwise `false` (saves memory)
         * @param store_lengths `true` to store edge lengths (default), otherwise `false` (saves memory)
         * @param reserve How many nodes to reserve memory for up-front to avoid `std::vector` resizes. It's fine if the true number of nodes in the tree exceeds this value (the `std::vector` will resize automatically), but get as close as possible for speed.
         */
        compact_tree(const char* const input, bool is_fn = true, bool store_labels = true, bool store_lengths = true, size_t reserve = 0);

        /**
         * Load a tree from a Newick file or `std::string`
         * @param input The filename or `std::string` of the tree to load
         * @param is_fn `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as an `std::string`
         * @param store_labels `true` to store node labels (default), otherwise `false` (saves memory)
         * @param store_lengths `true` to store edge lengths (default), otherwise `false` (saves memory)
         * @param reserve How many nodes to reserve memory for up-front to avoid `std::vector` resizes. It's fine if the true number of nodes in the tree exceeds this value (the `std::vector` will resize automatically), but get as close as possible for speed.
         */
        compact_tree(const std::string & input, bool is_fn = true, bool store_labels = true, bool store_lengths = true, size_t reserve = 0) : compact_tree(&input[0], is_fn, store_labels, store_lengths, reserve) {}

        /**
         * Copy constructor
         * @param o The other `compact_tree` to copy
         */
        compact_tree(const compact_tree & o) : parent(o.parent), children(o.children), label(o.label), length(o.length), num_leaves(o.num_leaves) {}

        /**
         * Add a child to an existing node
         * @param parent_node The parent node
         * @param new_label The label of the new child node
         * @param new_length The length of edge incident to the new child node
         * @return The newly-created child node
         */
        CT_NODE_T add_child(const CT_NODE_T parent_node, const std::string & new_label = EMPTY_STRING, CT_LENGTH_T new_length = ZERO_LENGTH) { if((num_leaves != 0) && !is_leaf(parent_node)) { ++num_leaves; } CT_NODE_T x = create_child(parent_node,has_labels(),has_edge_lengths()); if(!(new_label.empty())) { set_label(x,new_label); } if(std::abs(new_length) > ZERO_THRESH) { set_edge_length(x,new_length); } return x; }

        /**
         * Print the Newick string of the subtree rooted at a specific node (defaults to the root = entire tree)
         * @param out The output stream to print the Newick string to
         * @param node The root of the subtree to print the Newick string of
         * @param include_semicolon `true` to print a semicolon at the end of the Newick string (to end the tree), otherwise `false` (e.g. subtree)
         */
        void print_newick(std::ostream & out, CT_NODE_T node = ROOT_NODE, bool include_semicolon = true);

        /**
         * Return the Newick string of the subtree rooted at a specific node (defaults to the root = entire tree)
         * @param node The root of the subtree to print the Newick string of
         * @param include_semicolon `true` to print a semicolon at the end of the Newick string (to end the tree), otherwise `false` (e.g. subtree)
         */
        std::string get_newick(CT_NODE_T node = ROOT_NODE, bool include_semicolon = true) { std::ostringstream oss; print_newick(oss,node,include_semicolon); return oss.str(); }

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
        CT_NODE_T get_root() const { return ROOT_NODE; }

        /**
         * Check if a node is the root
         * @param node The node to check
         * @return `true` if `node` is the root, otherwise `false`
         */
        bool is_root(CT_NODE_T node) const { return node == ROOT_NODE; }

        /**
         * Check if a node is a leaf
         * @param node The node to check
         * @return `true` if `node` is a leaf, otherwise `false`
         */
        bool is_leaf(CT_NODE_T node) const { return children[node].size() == 0; }

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
         * Check if this tree is storing edge lengths
         * @return `true` if this tree is storing edge lengths, otherwise `false`
        */
        bool has_edge_lengths() const { return length.size() != 0; }

        /**
         * Clear all edge lengths in this tree
         * @param shrink `true` to deallocate the memory that was allocated to store the edge lengths, otherwise `false`
         */
        void clear_edge_lengths(bool shrink = true) { if(!length.empty()) { length.clear(); if(shrink) { length.shrink_to_fit(); } } }

        /**
         * Get the incident edge length of a node
         * @param node The node to get the incident edge length of
         * @return The incident edge length of `node`
         */
        CT_LENGTH_T get_edge_length(CT_NODE_T node) const { return has_edge_lengths() ? length[node] : (CT_LENGTH_T)0.; }

        /**
         * Get all edge lengths (return by reference)
         */
        const std::vector<CT_LENGTH_T> & get_edge_lengths() const { return length; }

        /**
         * Set the incident edge length of a node
         * @param node The node to set the incident edge length of
         * @param new_length The new edge length to set
         */
        void set_edge_length(CT_NODE_T node, CT_LENGTH_T new_length) {
            if(!has_edge_lengths()) { length = std::vector<CT_LENGTH_T>(get_num_nodes(), (CT_LENGTH_T)0.); } length[node] = new_length;
        }

        /**
         * Check if this tree is storing labels
         * @return `true` if this tree is storing labels, otherwise `false`
         */
        bool has_labels() const { return label.size() != 0; }

        /**
         * Clear all labels in this tree
         * @param shrink `true` to deallocate the memory that was allocated to store the labels, otherwise `false`
         */
        void clear_labels(bool shrink = true) { if(!label.empty()) { label.clear(); if(shrink) { label.shrink_to_fit(); } } }

        /**
         * Get the label of a node
         * @param node The node to get the label of
         * @return The label of `node`
         */
        const std::string & get_label(CT_NODE_T node) const { return has_labels() ? label[node] : EMPTY_STRING; }

        /**
         * Set the label of a node
         * @param node The node to set the label of
         * @param new_label The new label to set
         */
        void set_label(CT_NODE_T node, const std::string & new_label) {
            if(!has_labels()) { label = std::vector<std::string>(get_num_nodes(), ""); } label[node] = new_label;
        }

        /**
         * Get all labels (return by reference)
         * @return A `vector<string>` where the `i`-th value is the label of node `i`
         */
        const std::vector<std::string> & get_labels() const { return label; }

        /**
         * Replace all labels that match a user-provided mapping
         * @param label_map A mapping where keys are old labels and values are new labels (i.e., replace all occurrences of label `s` with `label_map[s]`)
         * @param include_internal `true` to include internal nodes in the label replacement, otherwise `false` to only replace leaf labels
         */
        void replace_labels(const std::unordered_map<std::string, std::string> & label_map, bool include_internal = true) {
            const size_t NUM_NODES = get_num_nodes(); CT_NODE_T curr; std::unordered_map<std::string,std::string>::const_iterator it; std::unordered_map<std::string,std::string>::const_iterator it_end = label_map.cend();
            for(curr = ROOT_NODE; curr < NUM_NODES; ++curr) {
                if(is_leaf(curr) || include_internal) {
                    it = label_map.find(label[curr]);
                    if(it != it_end) { label[curr] = it->second; }
                }
            }
        }

        /**
         * Return the data associated with a given node
         * @param i The node whose data to return
         * @return A `tuple<const string*, CT_LENGTH_T, CT_NODE_T, const vector<CT_NODE_T>*>` containing (1) a pointer to the label, (2) the incident edge length, (3) the parent, and (4) a pointer to the collection of children of node `i`
         */
        std::tuple<const std::string*, CT_LENGTH_T, CT_NODE_T, const std::vector<CT_NODE_T>*> operator[](CT_NODE_T i) const { return std::make_tuple(&get_label(i), get_edge_length(i), parent[i], &children[i]); }

        /**
         * Pre-order traversal iterator. The only guarantee is that a node will be visited before its children.
         * Currently, nodes will be visited in the order they appear in the original Newick string.
         */
        class preorder_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node;
            public:
                preorder_iterator(CT_NODE_T x) : node(x) {}
                preorder_iterator(const preorder_iterator & it) : node(it.node) {}
                preorder_iterator & operator=(const preorder_iterator & it) { node = it.node; return *this; }
                preorder_iterator & operator++() { ++node; return *this; }
                preorder_iterator operator++(int) { preorder_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const preorder_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const preorder_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };

        /**
         * Return an iterator referring to the first node of a pre-order traversal
         * @return An iterator referring to the first node of a pre-order traversal
         */
        preorder_iterator preorder_begin() { return preorder_iterator(ROOT_NODE); }

        /**
         * Return an iterator referring just past the last node of a pre-order traversal
         * @return An iterator referring just past the last node of a pre-order traversal
         */
        preorder_iterator preorder_end () { return preorder_iterator((CT_NODE_T)get_num_nodes()); }

        /**
         * Postorder traversal iterator. The only guarantee is that a node will be visited before its parent.
         * Currently, nodes will be visited in reverse of the order they appear in the original Newick string.
         */
        class postorder_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node;
            public:
                postorder_iterator(CT_NODE_T x) : node(x) {}
                postorder_iterator(const postorder_iterator & it) : node(it.node) {}
                postorder_iterator & operator=(const postorder_iterator & it) { node = it.node; return *this; }
                postorder_iterator & operator++() { --node; return *this; }
                postorder_iterator operator++(int) { postorder_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const postorder_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const postorder_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        /**
         * Return an iterator referring to the first node of a post-order traversal
         * @return An iterator referring to the first node of a post-order traversal
         */
        postorder_iterator postorder_begin() { return postorder_iterator((CT_NODE_T)(get_num_nodes() - 1)); }

        /**
         * Return an iterator referring just past the last node of a post-order traversal
         * @return An iterator referring just past the last node of a post-order traversal
         */
        postorder_iterator postorder_end() { return postorder_iterator(NULL_NODE); }

        /**
         * Levelorder traversal iterator. The only guarantee is that nodes will be visited in increasing order of depth (i.e., number of edges from the root).
         * Currently, nodes with the same depth will be visited in the order they appear in the original Newick string.
         * Note that the copy constructor and the post-increment `it++` operator will copy the BFS queue, which is slow and uses extra memory, so avoid both when possible.
         */
        class levelorder_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                std::queue<CT_NODE_T> q; compact_tree* tree_ptr;
            public:
                levelorder_iterator(compact_tree* const tp) : tree_ptr(tp) {}
                levelorder_iterator(const CT_NODE_T root, compact_tree* const tp) : tree_ptr(tp) { q.push(root); }
                levelorder_iterator(std::queue<CT_NODE_T> x, compact_tree* const tp) : q(x), tree_ptr(tp) {}
                levelorder_iterator(const levelorder_iterator & o) : q(o.q), tree_ptr(o.tree_ptr) {}
                levelorder_iterator & operator=(const levelorder_iterator & o) { q = o.q; tree_ptr = o.tree_ptr; return *this; }
                levelorder_iterator & operator++() { CT_NODE_T x = q.front(); std::vector<CT_NODE_T>::iterator it_end = tree_ptr->children[x].end(); for(std::vector<CT_NODE_T>::iterator it = tree_ptr->children[x].begin(); it != it_end; ++it) { q.push(*it); } q.pop(); return *this; }
                levelorder_iterator operator++(int) { levelorder_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const levelorder_iterator & rhs) const { return q == rhs.q; }
                bool operator!=(const levelorder_iterator & rhs) const { return q != rhs.q; }
                CT_NODE_T operator*() { return q.front(); }
        };

        /**
         * Return an iterator referring to the first node of a level-order traversal
         * @return An iterator referring to the first node of a level-order traversal
         */
        levelorder_iterator levelorder_begin() { return levelorder_iterator(ROOT_NODE, this); }

        /**
         * Return an iterator referring just past the last node of a level-order traversal
         * @return An iterator referring just past the last node of a level-order traversal
         */
        levelorder_iterator levelorder_end() { return levelorder_iterator(this); }

        /**
         * Iterate over the leaves of this tree.
         * Currently, leaves will be visited in the order they appear in the original Newick string.
         */
        class leaves_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node; const compact_tree* tree_ptr;
            public:
                leaves_iterator(CT_NODE_T x, const compact_tree* const tp) : node(x), tree_ptr(tp) {}
                leaves_iterator(const leaves_iterator & it) : node(it.node), tree_ptr(it.tree_ptr) {}
                leaves_iterator & operator=(const leaves_iterator & it) { node = it.node; tree_ptr = it.tree_ptr; return *this; }
                leaves_iterator & operator++() { CT_NODE_T num_nodes = tree_ptr->get_num_nodes(); while(((++node) < num_nodes) && !(tree_ptr->is_leaf(node))) {} return *this; }
                leaves_iterator operator++(int) { leaves_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const leaves_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const leaves_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };

        /**
         * Return an iterator referring to the first node of a leaf iteration
         * @return An iterator referring to the first node of a leaf iteration
         */
        leaves_iterator leaves_begin() { CT_NODE_T node = ROOT_NODE; CT_NODE_T num_nodes = get_num_nodes(); while(((++node) < num_nodes) && (!is_leaf(node))) {} return leaves_iterator(node, this); }

        /**
         * Return an iterator referring to just past the last node of a leaf iteration
         * @return An iterator referring to just past the last node of a leaf iteration
         */
        leaves_iterator leaves_end() { return leaves_iterator((CT_NODE_T)get_num_nodes(), this); }

        /**
         * Iterate over the children of a given node.
         * Currently, children will be visited in the order they appear in the original Newick string.
         */
        class children_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                std::vector<CT_NODE_T>::iterator it;
            public:
                children_iterator(std::vector<CT_NODE_T>::iterator x) : it(x) {}
                children_iterator(const children_iterator & o) : it(o.it) {}
                children_iterator & operator=(const children_iterator & o) { it = o.it; return *this; }
                children_iterator & operator++() { ++it; return *this; }
                children_iterator operator++(int) { children_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const children_iterator & rhs) const { return it == rhs.it; }
                bool operator!=(const children_iterator & rhs) const { return it != rhs.it; }
                CT_NODE_T operator*() { return *it; }
        };

        /**
         * Return an iterator referring to the first child of a node
         * @param node The node to iterate over the children of
         * @return An iterator referring to the first child of `node`
         */
        children_iterator children_begin(CT_NODE_T node) { return children_iterator(children[node].begin()); }

        /**
         * Return an iterator referring to just past the last child of a node
         * @param node The node to iterate over the children of
         * @return An iterator referring to just past the last child of `node`
         */
        children_iterator children_end(CT_NODE_T node) { return children_iterator(children[node].end()); }

        /**
         * Find and return the Most Recent Common Ancestor (MRCA) of a collection of nodes
         * @param nodes The nodes to find the MRCA of
         * @return The MRCA of the nodes in `nodes`
         */
        CT_NODE_T find_mrca(const std::unordered_set<CT_NODE_T> & nodes) const;

        /**
         * Extract the subtree rooted at a given node
         * @param node The node that is the root of the desired subtree
         * @return The subtree rooted at `node`
         */
        compact_tree extract_subtree(CT_NODE_T node);

        /**
         * Calculate the total branch length of this tree
         * @param include_internal `true` to include internal nodes, otherwise `false`
         * @param include_leaves `true` to include leaves, otherwise `false`
         * @return The total branch length of this tree
         */
        double calc_total_bl(bool include_internal = true, bool include_leaves = true) const {
            if(!(include_internal || include_leaves)) { return 0.; }
            double tot = 0; CT_NODE_T num_nodes = get_num_nodes();
            for(CT_NODE_T node = 0; node < num_nodes; ++node) {
                if(is_leaf(node)) { if(include_leaves) { tot += get_edge_length(node); } }
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
        double calc_avg_bl(bool include_internal = true, bool include_leaves = true) const {
            if(!(include_internal || include_leaves)) { return 0.; }
            double tot = 0; CT_NODE_T num_nodes = get_num_nodes(); CT_NODE_T count = 0;
            for(CT_NODE_T node = 0; node < num_nodes; ++node) {
                if(is_leaf(node)) { if(include_leaves) { tot += get_edge_length(node); ++count; } }
                else { if(include_internal) { tot += get_edge_length(node); ++count; } }
            }
            return tot / count;
        }

        /**
         * Calculate the (weighted) distance between two nodes
         * @param u The first node
         * @param v The second node
         * @return The (weighted) distance between `u` and `v`
         */
        double calc_dist(CT_NODE_T u, CT_NODE_T v) const {
            if(u == v) { return 0.; } if(u == parent[v]) { return get_edge_length(v); } if(v == parent[u]) { return get_edge_length(u); }
            std::unordered_map<CT_NODE_T, double> u_dists; std::unordered_map<CT_NODE_T, double> v_dists; CT_NODE_T c = u; CT_NODE_T p = parent[c]; std::unordered_map<CT_NODE_T, double>::iterator it;
            while(p != NULL_NODE) { u_dists[p] = u_dists[c] + get_edge_length(c); c = p; p = parent[p]; } c = v; p = parent[c];
            while(p != NULL_NODE) { v_dists[p] = v_dists[c] + get_edge_length(c); it = u_dists.find(p); if(it != u_dists.end()) { return it->second + v_dists[p]; } c = p; p = parent[p]; }
            return -1.; // error, shouldn't reach here
        }

        /**
         * Calculate the (weighted) distance matrix between all pairs of leaves
         * @return A `vector<pair<pair<node,node>,double>>` containing all pairwise leaf distances as `((u,v), distance)`
         */
        std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> calc_distance_matrix();
};

// print Newick string (currently recursive)
void compact_tree::print_newick(std::ostream & out, CT_NODE_T node, bool include_semicolon) {
    std::vector<CT_NODE_T>::iterator it_begin = children[node].begin(); std::vector<CT_NODE_T>::iterator it_end = children[node].end();
    for(std::vector<CT_NODE_T>::iterator it = it_begin; it != it_end; ++it) { out << ((it == it_begin) ? '(' : ','); print_newick(out, *it, false); }
    if(!is_leaf(node)) { out << ')'; }
    if(label.size() != 0) { out << label[node]; }
    if(length.size() != 0) { out << ':' << length[node]; }
    if(include_semicolon) { out << ';'; }
}

// find the MRCA of nodes
CT_NODE_T compact_tree::find_mrca(const std::unordered_set<CT_NODE_T> & nodes) const {
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

// calculate distance matrix
std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> compact_tree::calc_distance_matrix() {
    // set things up
    const size_t NUM_NODES = get_num_nodes(); const size_t N = get_num_leaves(); const size_t N_MINUS_1 = N - 1;
    const size_t N_CHOOSE_2 = ((N%2) == 0) ? ((N/2) * N_MINUS_1) : ((N_MINUS_1/2) * N);
    std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> dm; dm.reserve(N_CHOOSE_2);
    std::unordered_map<CT_NODE_T, std::unordered_map<CT_NODE_T, double>*> leaf_dists;
    std::vector<CT_NODE_T>::iterator ch_it; std::vector<CT_NODE_T>::iterator ch_it_2; std::vector<CT_NODE_T>::iterator ch_it_end;
    CT_NODE_T curr; CT_NODE_T child; CT_NODE_T child_2; std::unordered_map<CT_NODE_T, double>* curr_leaf_dists;
    std::unordered_map<CT_NODE_T, double>* child_leaf_dists; std::unordered_map<CT_NODE_T, double>::iterator dist_it; std::unordered_map<CT_NODE_T, double>::iterator dist_it_end;
    std::unordered_map<CT_NODE_T, double>* child_leaf_dists_2; std::unordered_map<CT_NODE_T, double>::iterator dist_it_2; std::unordered_map<CT_NODE_T, double>::iterator dist_it_2_end;

    // calculate pairwise distances
    for(curr = NUM_NODES-1; curr != NULL_NODE; --curr) {
        curr_leaf_dists = new std::unordered_map<CT_NODE_T, double>();
        leaf_dists.emplace(curr, curr_leaf_dists);

        // for leaves, they have 0 distance to themselves
        if(is_leaf(curr)) {
            curr_leaf_dists->emplace(curr, (double)0.);
        }

        // for internal nodes:
        else {
            // calculate all pairwise distances between leaves below this node
            for(ch_it = children[curr].begin(), ch_it_end = children[curr].end(); std::next(ch_it) != ch_it_end; ++ch_it) {
                child = *ch_it; child_leaf_dists = leaf_dists[child];
                for(ch_it_2 = std::next(ch_it); ch_it_2 != ch_it_end; ++ch_it_2) {
                    child_2 = *ch_it_2; child_leaf_dists_2 = leaf_dists[child_2];
                    for(dist_it = child_leaf_dists->begin(), dist_it_end = child_leaf_dists->end(); dist_it != dist_it_end; ++dist_it) {
                        for(dist_it_2 = child_leaf_dists_2->begin(), dist_it_2_end = child_leaf_dists_2->end(); dist_it_2 != dist_it_2_end; ++dist_it_2) {
                            dm.emplace_back(std::make_pair(std::make_pair(dist_it->first,dist_it_2->first), dist_it->second + dist_it_2->second + get_edge_length(child) + get_edge_length(child_2)));
                        }
                    }
                }
            }

            // calculate leaf distances to this node
            for(ch_it = children[curr].begin(), ch_it_end = children[curr].end(); ch_it != ch_it_end; ++ch_it) {
                child = *ch_it; child_leaf_dists = leaf_dists[child];
                for(dist_it = child_leaf_dists->begin(), dist_it_end = child_leaf_dists->end(); dist_it != dist_it_end; ++dist_it) {
                    curr_leaf_dists->emplace(dist_it->first, dist_it->second + length[child]);
                }
                delete child_leaf_dists;
            }
        }
    }
    delete leaf_dists[ROOT_NODE]; return dm;
}

// helper function to create new node and add as child to parent
CT_NODE_T compact_tree::create_child(const CT_NODE_T parent_node, bool store_labels, bool store_lengths) {
    tmp_node = parent.size(); parent.emplace_back(parent_node); children.emplace_back(std::vector<CT_NODE_T>());
    if(parent_node != NULL_NODE) { children[parent_node].emplace_back(tmp_node); }
    if(store_lengths) { length.emplace_back((CT_LENGTH_T)0); }
    if(store_labels) { label.emplace_back(""); }
    return tmp_node;
}

// extract a subtree
compact_tree compact_tree::extract_subtree(CT_NODE_T node) {
    compact_tree new_tree; std::stack<std::pair<CT_NODE_T, CT_NODE_T>> to_copy; to_copy.push(std::make_pair(node,0)); // first = old tree; second = new tree
    bool has_lengths = (length.size() != 0); bool has_labels = (label.size() != 0); new_tree.create_child(NULL_NODE, has_labels, has_lengths);
    if(has_lengths) { new_tree.length.emplace_back(ZERO_LENGTH); }
    if(has_labels) { new_tree.label.emplace_back(EMPTY_STRING); }
    std::pair<CT_NODE_T, CT_NODE_T> curr;
    while(!to_copy.empty()) {
        curr = to_copy.top(); to_copy.pop(); std::vector<CT_NODE_T>::iterator it_end = children[curr.first].end();
        if(has_lengths) { new_tree.length[curr.second] = length[curr.first]; }
        if(has_labels) { new_tree.label[curr.second] = label[curr.first]; }
        for(std::vector<CT_NODE_T>::iterator it = children[curr.first].begin(); it != it_end; ++it) {
            to_copy.push(std::make_pair(*it, new_tree.create_child(curr.second, has_labels, has_lengths)));
        }
    }
    return new_tree;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(const char* const input, bool is_fn, bool store_labels, bool store_lengths, size_t reserve) {
    // reserve space up-front (if given `reserve`) to reduce resizing (save time)
    if(reserve != 0) { parent.reserve(reserve); if(store_lengths) { length.reserve(reserve); } if(store_labels) { label.reserve(reserve); } }

    // set up file input: https://stackoverflow.com/a/17925143/2134991
    int fd = -1;
    size_t bytes_read = 0; size_t i;                            // variables to help with reading
    const size_t INPUT_LEN = strlen(input); size_t input_i = 0; // variables to help with reading from Newick string specifically
    char buf[IO_BUFFER_SIZE + 1];                               // buffer for reading
    char str_buf[STR_BUFFER_SIZE] = {}; size_t str_buf_i = 0;   // helper string buffer
    if(is_fn) {
        fd = open(input, O_RDONLY);
        if(fd == -1) {
            throw std::invalid_argument(ERROR_OPENING_FILE + std::string(": ") + std::string(input));
        }
        posix_fadvise(fd, 0, 0, 1); // FDADVICE_SEQUENTIAL
    }

    // set up initial Newick parsing state
    create_child(NULL_NODE, store_labels, store_lengths); // create empty root node
    CT_NODE_T curr_node = ROOT_NODE;                      // start traversal at root
    bool parse_length = false;                            // parsing a length right now?
    bool parse_label = false;                             // parsing a label right now?
    bool parse_label_single = false;                      // parsing a single-quote label right now?
    bool parse_label_double = false;                      // parsing a double-quote label right now?
    bool parse_comment = false;                           // parsing a comment [...] right now?

    // read Newick tree byte-by-byte
    while(true) {
        // either read from file or from Newick C string
        if(is_fn) {
            bytes_read = read(fd, buf, IO_BUFFER_SIZE);
        } else {
            if(input_i == INPUT_LEN) { // finished parsing string
                bytes_read = 0;
            } else {
                if((input_i + IO_BUFFER_SIZE) <= INPUT_LEN) {
                    bytes_read = IO_BUFFER_SIZE;
                } else {               // fewer than IO_BUFFER_SIZE bytes left in Newick string (input)
                    bytes_read = INPUT_LEN - input_i;
                }
                memcpy(buf, input+input_i, bytes_read); input_i += bytes_read;
            }
        }

        // handle done reading (!bytes_read) and read failed (bytes_read == -1)
        if(!bytes_read || bytes_read == (size_t)(-1)) {
            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + std::string(": ") + std::string(input));
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
                        if(store_lengths) { str_buf[str_buf_i] = (char)0; length[curr_node] = PARSE_LENGTH_FROM_C_STR(str_buf); }
                        parse_length = false; --i; break; // need to re-read this character

                    // edge comment (ignore for now)
                    case '[':
                        parse_comment = true; break;

                    // character within edge length
                    default:
                        if(store_lengths) { str_buf[str_buf_i++] = buf[i]; }
                        break;
                }
            }

            // currently parsing a node label
            else if(parse_label) {
                // parsing quoted label ('' or ""), so blindly add next char to label
                if(parse_label_single || parse_label_double) {
                    if(store_labels) { str_buf[str_buf_i++] = buf[i]; }
                    if(buf[i] == '\'' && parse_label_single) {
                        parse_label = false; parse_label_single = false;
                    } else if(buf[i] == '"' && parse_label_double) {
                        parse_label = false; parse_label_double = false;
                    }
                }

                // parsing non-quoted label, so check next char for validity before adding
                else {
                    switch(buf[i]) {
                        case ':': case ',': case ')': case ';':
                            parse_label = false; --i; break; // finished label, so need to re-read this character
                        default:
                            if(store_labels) { str_buf[str_buf_i++] = buf[i]; } break;
                    }
                }

                // if we finished parsing the label, finalize it
                if(!parse_label) {
                    if(store_labels) { str_buf[str_buf_i] = (char)0; label[curr_node] = str_buf; }
                    parse_label = false;
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
                        if(curr_node != (CT_NODE_T)0) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + std::string(": ") + std::string(input));
                        }
                        return;

                    // go to new child
                    case '(':
                        if(curr_node == NULL_NODE) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + std::string(": ") + std::string(input));
                        }
                        curr_node = create_child(curr_node, store_labels, store_lengths); break;

                    // go to parent
                    case ')':
                        curr_node = parent[curr_node]; break;

                    // go to new sibling
                    case ',':
                        if((curr_node == NULL_NODE) || (parent[curr_node] == NULL_NODE)) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + std::string(": ") + std::string(input));
                        }
                        curr_node = create_child(parent[curr_node], store_labels, store_lengths); break;

                    // node comment (ignore for now)
                    case '[':
                        parse_comment = true; break;

                    // edge length
                    case ':':
                        if(store_lengths) { str_buf_i = 0; }
                        parse_length = true; break;

                    // about to parse a node label in single quotes ('')
                    case '\'':
                        if(store_labels) { str_buf_i = 0; }
                        parse_label = true; parse_label_single = true; break;

                    // about to parse a node label in double quotes ("")
                    case '"':
                        if(store_labels) { str_buf_i = 0; }
                        parse_label = true; parse_label_double = true; break;

                    // about to start a node label without quotes
                    default:
                        if(store_labels) { str_buf_i = 0; }
                        parse_label = true; --i; break; // need to re-read this character (it's part of the label)
                }
            }
        }
    }
}
#endif
