/**
 * Header-only compact tree implementation
 * Niema Moshiri 2024
 */
#ifndef COMPACT_TREE_H
#define COMPACT_TREE_H

// include statements
#include <cstdint>       // std::uint32_t, std::uint64_t
#include <cstdlib>       // std::atof
#include <cstring>       // strcmp()
#include <fstream>       // std::ifstream
#include <iostream>      // std::cerr, std::cout, std::endl
#include <queue>         // std::queue
#include <sstream>       // std::stringstream
#include <stack>         // std::stack
#include <stdexcept>     // std::invalid_argument
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::pair
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
#define PARSE_LENGTH_FROM_STRING std::stod
#else
#define CT_LENGTH_T float
#define PARSE_LENGTH_FROM_STRING std::stof
#endif

// general constants
#define VERSION "0.0.2"
#define STR_BUFFER_SIZE 16384
const std::string EMPTY_STRING = "";
const CT_NODE_T ROOT_NODE = (CT_NODE_T)0;
const CT_NODE_T NULL_NODE = (CT_NODE_T)(-1);
const CT_LENGTH_T ZERO_LENGTH = (CT_LENGTH_T)0.;

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
        std::vector<CT_NODE_T> parent;                         // `parent[i]` is the parent of node `i`
        std::vector<std::pair<CT_NODE_T, CT_NODE_T>> children; // The children of node `i` are the nodes `children[i].first <= x <= children[i].second`
        std::vector<std::string> label;                        // `label[i]` is the label of node `i`
        std::vector<CT_LENGTH_T> length;                       // `length[i]` is the length of the edge incident to (i.e., going into) node `i`

        /**
         * Private helper function to print member variables (for debugging)
         */
        void print_members(std::ostream & out) {
            out << "parent: "; for(auto curr : parent) { out << curr << ' '; } out << std::endl;
            out << "children: "; for(auto curr : children) { out << '(' << curr.first << ',' << curr.second << ") "; } out << std::endl;
            out << "label: "; for(auto curr : label) { out << '\'' << curr << "' "; } out << std::endl;
            out << "length: "; for(auto curr : length) { out << curr << ' '; } out << std::endl;
        }

        /**
         * compact_tree helper member variables
         */
        size_t num_leaves = 0;   // cache the total number of leaves (avoid recalculation)

        /**
         * Helper constructor to create a completely empty compact_tree
         */
        compact_tree() {}

        /**
         * Helper function to create a new node and add it as a child to a given node
         * @param parent_node The parent node
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
        compact_tree(char* input, bool is_fn = true, bool store_labels = true, bool store_lengths = true, size_t reserve = 0);

        /**
         * Copy constructor
         * @param `o` The other `compact_tree` to copy
         */
        compact_tree(const compact_tree & o) : parent(o.parent), children(o.children), label(o.label), length(o.length), num_leaves(o.num_leaves) {}

        /**
         * Print the Newick string of the subtree rooted at a specific node
         * @param out The output stream to print the Newick string to
         * @param node The root of the subtree to print the Newick string of
         * @param print_semicolon `true` to print a semicolon at the end of the Newick string (to end the tree), otherwise `false` (e.g. subtree)
         */
        void print_newick(std::ostream & out, CT_NODE_T node, bool print_semicolon = true);

        /**
         * Print the Newick string of the entire tree
         * @param out The output stream to print the Newick string to
         */
        void print_newick(std::ostream & out) { print_newick(out, ROOT_NODE, true); }

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
        bool is_leaf(CT_NODE_T node) const { return children[node].first == NULL_NODE; }

        /**
         * Get the parent of a node
         * @param node The node to get the parent of
         * @return The parent of `node`
         */
        CT_NODE_T get_parent(CT_NODE_T node) const { return parent[node]; }

        /**
         * Check if this tree is storing edge lengths
         * @return `true` if this tree is storing edge lengths, otherwise `false`
        */
        bool has_edge_lengths() const { return length.size() != 0; }

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
         * @param The new edge length to set
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
        preorder_iterator preorder_begin() { return preorder_iterator(ROOT_NODE); }
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
         * Iterate over the leaves of this tree
         */
        class leaves_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node; const compact_tree* const tree_ptr;
            public:
                leaves_iterator(CT_NODE_T x, const compact_tree* const tp) : node(x), tree_ptr(tp) {}
                leaves_iterator(const leaves_iterator & it) : node(it.node), tree_ptr(it.tree_ptr) {}
                leaves_iterator & operator++() { CT_NODE_T num_nodes = tree_ptr->get_num_nodes(); while(((++node) < num_nodes) && !(tree_ptr->is_leaf(node))) {} return *this; }
                leaves_iterator operator++(int) { leaves_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const leaves_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const leaves_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        leaves_iterator leaves_begin() { CT_NODE_T node = ROOT_NODE; CT_NODE_T num_nodes = get_num_nodes(); while(((++node) < num_nodes) && (!is_leaf(node))) {} return leaves_iterator(node, this); }
        leaves_iterator leaves_end() { return leaves_iterator((CT_NODE_T)get_num_nodes(), this); }

        /**
         * Iterate over the children of a given node
         */
        class children_iterator : public std::iterator<std::input_iterator_tag, CT_NODE_T> {
            private:
                CT_NODE_T node; const compact_tree* const tree_ptr;
            public:
                children_iterator(CT_NODE_T x, const compact_tree* const tp) : node(x), tree_ptr(tp) {}
                children_iterator(const children_iterator & it) : node(it.node), tree_ptr(it.tree_ptr) {}
                children_iterator & operator++() { node = (node == tree_ptr->children[tree_ptr->parent[node]].second) ? NULL_NODE : (node + 1); return *this; }
                children_iterator operator++(int) { children_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const children_iterator & rhs) const { return node == rhs.node; }
                bool operator!=(const children_iterator & rhs) const { return node != rhs.node; }
                CT_NODE_T operator*() { return node; }
        };
        children_iterator children_begin(CT_NODE_T node) { return children_iterator(children[node].first, this); }
        children_iterator children_end() { return children_iterator(NULL_NODE, this); }

        /**
         * Find and return the Most Recent Common Ancestor (MRCA) of a collection of nodes
         * @param nodes The nodes to find the MRCA of
         * @return The MRCA of the nodes in `nodes`
         */
        CT_NODE_T find_mrca(const std::unordered_set<CT_NODE_T> & nodes);

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
        double calc_total_bl(bool include_internal = true, bool include_leaves = true) {
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

// print Newick string (currently recursive)
void compact_tree::print_newick(std::ostream & out, CT_NODE_T node, bool print_semicolon) {
    auto it_begin = children_begin(node); auto it_end = children_end();
    for(auto it = it_begin; it != it_end; ++it) { out << ((it == it_begin) ? '(' : ','); print_newick(out, *it, false); }
    if(!is_leaf(node)) { out << ')'; }
    if(label.size() != 0) { out << label[node]; }
    if(length.size() != 0) { out << ':' << length[node]; }
    if(print_semicolon) { out << ';'; }
}

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

// extract a subtree
compact_tree compact_tree::extract_subtree(CT_NODE_T node) {
    compact_tree new_tree; std::stack<std::pair<CT_NODE_T, CT_NODE_T>> to_copy; to_copy.push(std::make_pair(node,0)); // first = old tree; second = new tree
    bool has_lengths = (length.size() != 0); bool has_labels = (label.size() != 0); auto it_end = children_end();
    new_tree.parent.emplace_back(NULL_NODE); new_tree.children.emplace_back(std::make_pair(NULL_NODE,NULL_NODE));
    if(has_lengths) { new_tree.length.emplace_back(ZERO_LENGTH); }
    if(has_labels) { new_tree.label.emplace_back(EMPTY_STRING); }
    std::pair<CT_NODE_T, CT_NODE_T> curr;
    while(!to_copy.empty()) {
        curr = to_copy.top(); to_copy.pop();
        if(has_lengths) { new_tree.length[curr.second] = length[curr.first]; }
        if(has_labels) { new_tree.label[curr.second] = label[curr.first]; }
        for(auto it = children_begin(curr.first); it != it_end; ++it) {
            to_copy.push(std::make_pair(*it, new_tree.create_child(curr.second, has_labels, has_lengths)));
        }
    }
    return new_tree;
}

// helper function to create a child (doesn't update child stuff in parent!!!)
CT_NODE_T compact_tree::create_child(const CT_NODE_T parent_node, bool store_labels, bool store_lengths) {
    parent.emplace_back(parent_node); children.emplace_back(std::make_pair(NULL_NODE, NULL_NODE));
    if(store_labels) { label.emplace_back(EMPTY_STRING); }
    if(store_lengths) { length.emplace_back(ZERO_LENGTH); }
    return parent.size() - 1;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(char* input, bool is_fn, bool store_labels, bool store_lengths, size_t reserve) {
    // reserve space up-front (if given `reserve`) to reduce resizing (save time)
    if(reserve != 0) {
        parent.reserve(reserve); children.reserve(reserve);
        if(store_lengths) { length.reserve(reserve); }
        if(store_labels) { label.reserve(reserve); }
    }

    // load entire Newick string up-front
    std::string newick;
    if(is_fn) {
        std::ifstream in(input); std::getline(in >> std::ws, newick);
    } else {
        std::istringstream in(input); std::getline(in >> std::ws, newick);
    }
    size_t newick_len = newick.length();

    // preprocess Newick string: find each the "next" index of each '(' or ','
    std::unordered_map<size_t, size_t> newick_next; std::vector<size_t> curr_left; size_t newick_semicolon_ind = 0;
    for(size_t i = 0; i < newick_len; ++i) {
        switch(newick[i]) {
            case '(':
                curr_left.emplace_back(i); break;
            case ',':
                newick_next[curr_left.back()] = i; curr_left.back() = i; break;
            case ')':
                newick_next[curr_left.back()] = i; curr_left.pop_back(); break;
            case ';':
                newick_semicolon_ind = i; break;
            default:
                break;
        };
    }
    if(!curr_left.empty() || newick_semicolon_ind == 0) {
        throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
    }

    // trim terminating whitespace and ';'
    for(size_t i = newick_semicolon_ind; i < newick_len; ++i) {
        newick.pop_back(); // should be O(1)
    }
    newick_len = newick.length(); // recalculate updated length

    // parse Newick string in level-order
    std::queue<std::pair<size_t, CT_NODE_T>> to_visit; to_visit.emplace(std::make_pair(0, ROOT_NODE));
    create_child(NULL_NODE, store_labels, store_lengths); std::pair<size_t, CT_NODE_T> curr_pair; size_t tmp_ind; CT_NODE_T tmp_node; std::string tmp_s;
    while(!to_visit.empty()) {
        // get next node and add all of its children to the queue
        curr_pair = to_visit.front(); to_visit.pop(); tmp_ind = curr_pair.first;
        while(newick[tmp_ind] == ',' || newick[tmp_ind] == '(') {
            tmp_node = create_child(curr_pair.second, store_labels, store_lengths); children[curr_pair.second].second = tmp_node;
            if(children[curr_pair.second].first == NULL_NODE) { children[curr_pair.second].first = tmp_node; }
            to_visit.emplace(std::make_pair(tmp_ind+1, tmp_node)); tmp_ind = newick_next[tmp_ind];
        }

        // parse label and edge
        if(store_labels || store_lengths) {
            if(newick[tmp_ind] == ')') { ++tmp_ind; }

            // parse label (if it exists)
            if(store_labels) {
                while(tmp_ind < newick_len && newick[tmp_ind] != ':' && newick[tmp_ind] != ',' && newick[tmp_ind] != ')' && newick[tmp_ind] != ';') {
                    label[curr_pair.second] += newick[tmp_ind++];
                }
            }

            // parse edge length (if it exists)
            if(store_lengths) {
                tmp_s.clear(); if(newick[tmp_ind] == ':') { ++tmp_ind; }
                while(tmp_ind < newick_len && newick[tmp_ind] != ',' && newick[tmp_ind] != ')' && newick[tmp_ind] != ';') {
                    tmp_s += newick[tmp_ind++];
                }
                if(tmp_s.length() != 0) {
                    length[curr_pair.second] = PARSE_LENGTH_FROM_STRING(tmp_s);
                }
            }
        }
    }
}
#endif
