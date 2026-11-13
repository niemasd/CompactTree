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
#include <cerrno>        // errno
#include <fcntl.h>       // O_RDONLY, open(), posix_fadvise()
#include <iostream>      // std::ostream
#include <limits>        // std::numeric_limits<T>::max
#include <queue>         // std::queue
#include <sstream>       // std::ostringstream
#include <stack>         // std::stack
#include <stdexcept>     // std::invalid_argument
#include <string>        // std::string
#include <tuple>         // std::tuple
#include <unistd.h>      // read(), lseek(), close()
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
#define COMPACTTREE_VERSION "1.1.0"
#define IO_BUFFER_SIZE 16384
#define ROOT_NODE ((CT_NODE_T)0)
#define NULL_NODE (std::numeric_limits<CT_NODE_T>::max())
#define ZERO_LENGTH ((CT_LENGTH_T)0.)
#define ZERO_THRESH ((CT_LENGTH_T)0.000001)

// compact_tree class
class compact_tree {
    private:
        /**
         * compact_tree important member variables
         */
        std::vector<CT_NODE_T> parent;                // `parent[i]` is the parent of node `i`
        std::vector<CT_NODE_T> first_child;           // `first_child[i]` is the first child of node `i`
        std::vector<CT_NODE_T> last_child;            // `last_child[i]` is the last child of node `i`
        std::vector<std::string> label;               // `label[i]` is the label of node `i`
        std::vector<CT_LENGTH_T> length;              // `length[i]` is the length of the edge incident to (i.e., going into) node `i`

        /**
         * compact_tree helper member variables
         */
        size_t num_leaves = 0;   // cache the total number of leaves (avoid recalculation)

        /**
         * Helper constructor to create a completely empty compact_tree
         */
        compact_tree() {}

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
        compact_tree(const compact_tree & o) : parent(o.parent), first_child(o.first_child), last_child(o.last_child), label(o.label), length(o.length), num_leaves(o.num_leaves) {}

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
        std::string get_newick(CT_NODE_T node = ROOT_NODE, bool include_semicolon = true) { std::ostringstream oss; oss.precision(std::numeric_limits<CT_LENGTH_T>::max_digits10); print_newick(oss,node,include_semicolon); return oss.str(); }

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
        bool is_leaf(CT_NODE_T node) const { return first_child[node] == NULL_NODE; }

        /**
         * Get the parent of a node
         * @param node The node to get the parent of
         * @return The parent of `node`
         */
        CT_NODE_T get_parent(CT_NODE_T node) const { return parent[node]; }

        /**
         * Get the first child of a node
         * @param node The node to get the first child of
         * @return The first child of `node`
         */
        CT_NODE_T get_first_child(CT_NODE_T node) const { return first_child[node]; }

        /**
         * Get the last child of a node
         * @param node The node to get the last child of
         * @return The last child of `node`
         */
        CT_NODE_T get_last_child(CT_NODE_T node) const { return last_child[node]; }

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
        const std::string & get_label(CT_NODE_T node) const { if(has_labels()) { return label[node]; } else { static const std::string empty_string; return empty_string; } }

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
            if(!has_labels()) { return; }
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
         * @return A `tuple<const string*, CT_LENGTH_T>` containing (1) a pointer to the label, and (2) the incident edge length of node `i`
         */
        std::tuple<const std::string*, CT_LENGTH_T> operator[](CT_NODE_T i) const { return std::make_tuple(&get_label(i), get_edge_length(i)); }

        /**
         * Pre-order traversal iterator. The only guarantee is that a node will be visited before its children.
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
                levelorder_iterator & operator++() { CT_NODE_T x = q.front(); if(tree_ptr->first_child[x] != NULL_NODE) { for(CT_NODE_T child = tree_ptr->first_child[x]; child <= tree_ptr->last_child[x]; ++child) { q.push(child); } } q.pop(); return *this; }
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
        leaves_iterator leaves_begin() { CT_NODE_T node = ROOT_NODE; CT_NODE_T num_nodes = get_num_nodes(); if(!is_leaf(node)) { while(((++node) < num_nodes) && (!is_leaf(node))) {} } return leaves_iterator(node, this); }

        /**
         * Return an iterator referring to just past the last node of a leaf iteration
         * @return An iterator referring to just past the last node of a leaf iteration
         */
        leaves_iterator leaves_end() { return leaves_iterator((CT_NODE_T)get_num_nodes(), this); }

        /**
         * Iterate over the children of a given node.
         */
        class children_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node;
            public:
                children_iterator(CT_NODE_T x) : node(x) {}
                children_iterator(const children_iterator & o) : node(o.node) {}
                children_iterator & operator=(const children_iterator & o) { node = o.node; return *this; }
                children_iterator & operator++() { ++node; return *this; }
                children_iterator operator++(int) { children_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const children_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const children_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };

        /**
         * Return an iterator referring to the first child of a node
         * @param node The node to iterate over the children of
         * @return An iterator referring to the first child of `node`
         */
        children_iterator children_begin(CT_NODE_T node) { return children_iterator(first_child[node]); }

        /**
         * Return an iterator referring to just past the last child of a node
         * @param node The node to iterate over the children of
         * @return An iterator referring to just past the last child of `node`
         */
        children_iterator children_end(CT_NODE_T node) { return children_iterator((last_child[node] == NULL_NODE) ? NULL_NODE : last_child[node] + 1); }

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
            return (count == 0) ? 0. : (tot / count);
        }

        /**
         * Calculate the (weighted) distance between two nodes
         * @param u The first node
         * @param v The second node
         * @return The (weighted) distance between `u` and `v`
         */
        double calc_dist(CT_NODE_T u, CT_NODE_T v) const {
            if(u == v) { return 0.; } if(u == parent[v]) { return get_edge_length(v); } if(v == parent[u]) { return get_edge_length(u); }
            std::unordered_map<CT_NODE_T, double> u_dists; u_dists[u] = 0.; std::unordered_map<CT_NODE_T, double> v_dists; CT_NODE_T c = u; CT_NODE_T p = parent[c]; std::unordered_map<CT_NODE_T, double>::iterator it;
            while(p != NULL_NODE) { u_dists[p] = u_dists[c] + get_edge_length(c); c = p; p = parent[p]; } c = v; p = parent[c];
            it = u_dists.find(c); if(it != u_dists.end()) { return it->second + v_dists[c];}
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
inline void compact_tree::print_newick(std::ostream & out, CT_NODE_T node, bool include_semicolon) {
    if(!is_leaf(node)) { for(CT_NODE_T child = first_child[node]; child <= last_child[node]; ++child) { out << ((child == first_child[node]) ? '(' : ','); print_newick(out, child, false); } out << ')'; }
    if(label.size() != 0) {
       const std::string & curr_label = label[node];
        if(curr_label.find_first_of(" \t\n\r\f\v()[],:;'\"") != std::string::npos) {
            out << '\'';
            for(size_t i = 0; i < curr_label.size(); ++i) {
                if(curr_label[i] == '\'') { out << "''"; }
                else { out << curr_label[i]; }
            }
            out << '\'';
        } else {
            out << curr_label;
        }
    }
    if(length.size() != 0) { out << ':' << length[node]; }
    if(include_semicolon) { out << ';'; }
}

// find the MRCA of nodes
inline CT_NODE_T compact_tree::find_mrca(const std::unordered_set<CT_NODE_T> & nodes) const {
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
inline std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> compact_tree::calc_distance_matrix() {
    // set things up
    const size_t NUM_NODES = get_num_nodes(); const size_t N = get_num_leaves(); const size_t N_MINUS_1 = N - 1;
    const size_t N_CHOOSE_2 = ((N%2) == 0) ? ((N/2) * N_MINUS_1) : ((N_MINUS_1/2) * N);
    std::vector<std::pair<std::pair<CT_NODE_T,CT_NODE_T>, double>> dm; dm.reserve(N_CHOOSE_2);
    std::unordered_map<CT_NODE_T, std::unordered_map<CT_NODE_T, double>*> leaf_dists;
    CT_NODE_T ch_it; CT_NODE_T ch_it_2;
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
            for(ch_it = first_child[curr]; ch_it < last_child[curr]; ++ch_it) {
                child = ch_it; child_leaf_dists = leaf_dists[child];
                for(ch_it_2 = ch_it+1; ch_it_2 <= last_child[curr]; ++ch_it_2) {
                    child_2 = ch_it_2; child_leaf_dists_2 = leaf_dists[child_2];
                    for(dist_it = child_leaf_dists->begin(), dist_it_end = child_leaf_dists->end(); dist_it != dist_it_end; ++dist_it) {
                        for(dist_it_2 = child_leaf_dists_2->begin(), dist_it_2_end = child_leaf_dists_2->end(); dist_it_2 != dist_it_2_end; ++dist_it_2) {
                            dm.emplace_back(std::make_pair(std::make_pair(dist_it->first,dist_it_2->first), dist_it->second + dist_it_2->second + get_edge_length(child) + get_edge_length(child_2)));
                        }
                    }
                }
            }

            // calculate leaf distances to this node
            for(ch_it = first_child[curr]; ch_it <= last_child[curr]; ++ch_it) {
                child = ch_it; child_leaf_dists = leaf_dists[child];
                for(dist_it = child_leaf_dists->begin(), dist_it_end = child_leaf_dists->end(); dist_it != dist_it_end; ++dist_it) {
                    curr_leaf_dists->emplace(dist_it->first, dist_it->second + get_edge_length(child));
                }
                delete child_leaf_dists;
            }
        }
    }
    delete leaf_dists[ROOT_NODE]; return dm;
}

// extract a subtree
inline compact_tree compact_tree::extract_subtree(CT_NODE_T node) {
    compact_tree new_tree; std::stack<std::pair<CT_NODE_T, CT_NODE_T>> to_copy; to_copy.push(std::make_pair(node,0)); // first = old tree; second = new tree
    bool has_lengths = (length.size() != 0); bool has_labels = (label.size() != 0);
    new_tree.parent.emplace_back(NULL_NODE); new_tree.first_child.emplace_back(NULL_NODE); new_tree.last_child.emplace_back(NULL_NODE);
    if(has_lengths) { new_tree.length.emplace_back(ZERO_LENGTH); }
    if(has_labels) { new_tree.label.emplace_back(""); }
    std::pair<CT_NODE_T, CT_NODE_T> curr;
    while(!to_copy.empty()) {
        curr = to_copy.top(); to_copy.pop();
        if(has_lengths) { new_tree.length[curr.second] = length[curr.first]; }
        if(has_labels) { new_tree.label[curr.second] = label[curr.first]; }
        if(!is_leaf(curr.first)) {
            new_tree.first_child[curr.second] = (CT_NODE_T)new_tree.parent.size();
            new_tree.last_child[curr.second] = new_tree.first_child[curr.second] + last_child[curr.first] - first_child[curr.first];
            for(CT_NODE_T child = first_child[curr.first]; child <= last_child[curr.first]; ++child) {
                CT_NODE_T new_child = (CT_NODE_T)new_tree.parent.size();
                new_tree.parent.emplace_back(curr.second); new_tree.first_child.emplace_back(NULL_NODE); new_tree.last_child.emplace_back(NULL_NODE);
                if(has_lengths) { new_tree.length.emplace_back(ZERO_LENGTH); }
                if(has_labels) { new_tree.label.emplace_back(""); }
                to_copy.push(std::make_pair(child,new_child));
            }
        }
    }
    return new_tree;
}

// compact_tree constructor (putting it last because it's super long)
inline compact_tree::compact_tree(const char* const input, bool is_fn, bool store_labels, bool store_lengths, size_t reserve) {
    // reserve space up-front (if given `reserve`) to reduce resizing (save time)
    std::vector<CT_NODE_T> num_children;
    if(reserve != 0) { parent.reserve(reserve); first_child.reserve(reserve); last_child.reserve(reserve); num_children.reserve(reserve); if(store_lengths) { length.reserve(reserve); } if(store_labels) { label.reserve(reserve); } }

    // set up file input: https://stackoverflow.com/a/17925143/2134991
    int fd = -1;
    struct fd_guard { int* fd; ~fd_guard() { if(*fd != -1) { close(*fd); } } } close_fd{&fd};
    size_t bytes_read = 0; size_t i; // variables to help with reading
    const size_t INPUT_LEN = strlen(input); size_t input_i = 0; // variables to help with reading from Newick string specifically
    char buf[IO_BUFFER_SIZE + 1]; // buffer for reading
    std::string str_buf; // helper string buffer
    if(is_fn) {
        fd = open(input, O_RDONLY);
        if(fd == -1) {
            throw std::invalid_argument("Error opening file" + std::string(": ") + std::string(input));
        }
        posix_fadvise(fd, 0, 0, 1); // FDADVICE_SEQUENTIAL
    }

    // parse the Newick string twice: first to count children, then to create the tree
    for(size_t pass = 0; pass < 2; ++pass) {
        // set up initial Newick parsing state
        size_t curr_node = ROOT_NODE;                       // current node (temporary ID in pass 1, final ID in pass 2)
        size_t parse_node = ROOT_NODE;                      // temporary preorder ID (pass 2 only)
        size_t next_parse_node = 1;                         // next temporary preorder ID (pass 2 only)
        size_t next_node = 1;                               // next available final node ID (pass 2 only)
        std::stack<std::pair<size_t, size_t>> parse_parent; // first = parent ID; second = temporary preorder ID
        bool parse_done = false;                            // finished parsing current pass?
        bool parse_length = false;                          // parsing a length right now?
        bool parse_label = false;                           // parsing a label right now?
        bool parse_label_single = false;                    // parsing a single-quote label right now?
        bool parse_label_double = false;                    // parsing a double-quote label right now?
        bool parse_label_quote_pending = false;             // encountered a possible closing quote (might be escaped)
        bool parse_comment = false;                         // parsing a comment [...] right now?
        str_buf.clear();

        // initialize storage for the current pass
        if(pass == 0) {
            num_children.emplace_back(0); // create empty root node
        } else {
            parent.resize(num_children.size(), NULL_NODE);
            first_child.resize(num_children.size(), NULL_NODE);
            last_child.resize(num_children.size(), NULL_NODE);
            if(store_lengths) { length.resize(num_children.size(), ZERO_LENGTH); }
            if(store_labels) { label.resize(num_children.size(), ""); }
        }

        // read Newick tree byte-by-byte
        while(!parse_done) {
            // either read from file or from Newick C string
            if(is_fn) {
                ssize_t read_result;
                do {
                    read_result = read(fd, buf, IO_BUFFER_SIZE);
                } while((read_result == -1) && (errno == EINTR));
                bytes_read = (read_result < 0) ? (size_t)(-1) : (size_t)read_result;
            } else {
                if(input_i == INPUT_LEN) { // finished parsing string
                    bytes_read = 0;
                } else {
                    if((input_i + IO_BUFFER_SIZE) <= INPUT_LEN) {
                        bytes_read = IO_BUFFER_SIZE;
                    } else { // fewer than IO_BUFFER_SIZE bytes left in Newick string (input)
                        bytes_read = INPUT_LEN - input_i;
                    }
                    memcpy(buf, input+input_i, bytes_read); input_i += bytes_read;
                }
            }

            // handle done reading (!bytes_read) and read failed (bytes_read == -1)
            if(!bytes_read || bytes_read == (size_t)(-1)) {
                throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
            }

            // iterate over the characters in the buffer
            for(i = 0; (i < bytes_read) && !parse_done; ++i) {
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
                            if((pass == 1) && store_lengths) { length[curr_node] = PARSE_LENGTH_FROM_C_STR(str_buf.c_str()); }
                            parse_length = false; --i; break; // need to re-read this character

                        // edge comment (ignore for now)
                        case '[':
                            parse_comment = true; break;

                        // character within edge length
                        default:
                            if((pass == 1) && store_lengths) { str_buf.push_back(buf[i]); } break;
                    }
                }

                // currently parsing a node label
                else if(parse_label) {
                    // parsing quoted label ('' or ""), so blindly add next char to label
                    if(parse_label_single || parse_label_double) {
                        if(parse_label_quote_pending) {
                            if((parse_label_single && (buf[i] == '\'')) || (parse_label_double && (buf[i] == '"'))) {
                                if((pass == 1) && store_labels) { str_buf.push_back(buf[i]); }; parse_label_quote_pending = false;
                            } else {
                                parse_label = false; parse_label_single = false; parse_label_double = false; parse_label_quote_pending = false; --i;
                            }
                        } else if((parse_label_single && (buf[i] == '\'')) || (parse_label_double && (buf[i] == '"'))) {
                            parse_label_quote_pending = true;
                        } else {
                            if((pass == 1) && store_labels) { str_buf.push_back(buf[i]); }
                        }
                    }

                    // parsing non-quoted label, so check next char for validity before adding
                    else {
                        switch(buf[i]) {
                            case ':': case ',': case ')': case ';':
                                parse_label = false; --i; break; // finished label, so need to re-read this character
                            case '[':
                                parse_comment = true; break;
                            default:
                                if((pass == 1) && store_labels) { str_buf.push_back(buf[i]); }; break;
                        }
                    }

                    // if we finished parsing the label, finalize it
                    if(!parse_label) {
                        if((pass == 1) && store_labels) { label[curr_node] = str_buf; }; parse_label = false;
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
                            if((curr_node != ROOT_NODE) || !parse_parent.empty()) {
                                throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                            }
                            if(pass == 0) {
                                parse_done = true;
                                break;
                            }
                            if((parse_node != ROOT_NODE) || (next_parse_node != num_children.size()) || (next_node != num_children.size())) {
                                throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                            }
                            return;

                        // go to new child
                        case '(':
                            if(pass == 0) {
                                if((num_children[curr_node] == (NULL_NODE - 1)) || (num_children.size() >= NULL_NODE)) {
                                    throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                                }
                                ++num_children[curr_node];
                                parse_parent.push(std::make_pair(curr_node,curr_node));
                                num_children.emplace_back(0);
                                curr_node = num_children.size() - 1;
                            } else {
                                if((parse_node >= num_children.size()) || (num_children[parse_node] == 0) || (next_node > num_children.size()) || ((size_t)num_children[parse_node] > (num_children.size() - next_node)) || (next_parse_node >= num_children.size())) {
                                    throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                                }
                                first_child[curr_node] = (CT_NODE_T)next_node;
                                last_child[curr_node] = (CT_NODE_T)(next_node + num_children[parse_node] - 1);
                                parse_parent.push(std::make_pair(curr_node,parse_node));
                                curr_node = next_node;
                                parent[curr_node] = (CT_NODE_T)parse_parent.top().first;
                                next_node += num_children[parse_node];
                                parse_node = next_parse_node++;
                            }
                            break;

                        // go to parent
                        case ')':
                            if(parse_parent.empty() || ((pass == 1) && (curr_node != last_child[parse_parent.top().first]))) {
                                throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                            }
                            curr_node = parse_parent.top().first;
                            parse_node = parse_parent.top().second;
                            parse_parent.pop();
                            break;

                        // go to new sibling
                        case ',':
                            if(parse_parent.empty()) {
                                throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                            }
                            if(pass == 0) {
                                if((num_children[parse_parent.top().first] == (NULL_NODE - 1)) || (num_children.size() >= NULL_NODE)) {
                                    throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                                }
                                ++num_children[parse_parent.top().first];
                                num_children.emplace_back(0);
                                curr_node = num_children.size() - 1;
                            } else {
                                if((curr_node >= last_child[parse_parent.top().first]) || (next_parse_node >= num_children.size())) {
                                    throw std::invalid_argument((is_fn ? "Invalid Newick file" : "Invalid Newick string") + std::string(": ") + std::string(input));
                                }
                                ++curr_node;
                                parent[curr_node] = (CT_NODE_T)parse_parent.top().first;
                                parse_node = next_parse_node++;
                            }
                            break;

                        // node comment (ignore for now)
                        case '[':
                            parse_comment = true; break;

                        // edge length
                        case ':':
                            if(store_lengths) { str_buf.clear(); } parse_length = true; break;

                        // about to parse a node label in single quotes ('')
                        case '\'': 
                            if(store_labels) { str_buf.clear(); }
                            parse_label_quote_pending = false; parse_label = true; parse_label_single = true; break;

                        // about to parse a node label in double quotes ("")
                        case '"':
                            if(store_labels) { str_buf.clear(); }
                            parse_label_quote_pending = false; parse_label = true; parse_label_double = true; break;

                        // about to start a node label without quotes
                        default:
                            if(store_labels) { str_buf.clear(); }
                            parse_label = true; --i; break; // need to re-read this character (it's part of the label)
                    }
                }
            }
        }
        // reset input for the second pass
        if(pass == 0) {
            if(is_fn) {
                if(lseek(fd, 0, SEEK_SET) == (off_t)-1) {
                    throw std::invalid_argument("Invalid Newick file" + std::string(": ") + std::string(input));
                }
            } else {
                input_i = 0;
            }
        }
    }
}
#endif
