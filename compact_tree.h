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
#include <fcntl.h>       // O_RDONLY, open(), posix_fadvise()
#include <queue>         // std::queue
#include <stack>         // std::stack
#include <stdexcept>     // std::invalid_argument
#include <string>        // std::string
#include <tuple>         // std::tuple
#include <unistd.h>      // read()
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
#define PARSE_LENGTH_FROM_C_STR std::atof
#define PARSE_LENGTH_FROM_STRING std::stod
#else
#define CT_LENGTH_T float
#define PARSE_LENGTH_FROM_C_STR (float)std::atof
#define PARSE_LENGTH_FROM_STRING std::stof
#endif

// general constants
#define VERSION "0.0.2"
#define IO_BUFFER_SIZE 16384
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
         * Load a tree from a Newick file or `std::string`
         * @param input The filename or `std::string` of the tree to load
         * @param is_fn `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as an `std::string`
         * @param store_labels `true` to store node labels (default), otherwise `false` (saves memory)
         * @param store_lengths `true` to store edge lengths (default), otherwise `false` (saves memory)
         * @param reserve How many nodes to reserve memory for up-front to avoid `std::vector` resizes. It's fine if the true number of nodes in the tree exceeds this value (the `std::vector` will resize automatically), but get as close as possible for speed.
         */
        compact_tree(std::string & input, bool is_fn = true, bool store_labels = true, bool store_lengths = true, size_t reserve = 0) : compact_tree(&input[0], is_fn, store_labels, store_lengths, reserve) {}

        /**
         * Copy constructor
         * @param o The other `compact_tree` to copy
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
        bool is_leaf(CT_NODE_T node) const { return children[node].size() == 0; }

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
        preorder_iterator preorder_end () { return preorder_iterator((CT_NODE_T)get_num_nodes()); }

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
                std::vector<CT_NODE_T>::iterator it;
            public:
                children_iterator(std::vector<CT_NODE_T>::iterator x) : it(x) {}
                children_iterator(const children_iterator & o) : it(o.it) {}
                children_iterator & operator++() { ++it; return *this; }
                children_iterator operator++(int) { children_iterator tmp(*this); operator++(); return tmp; }
                bool operator==(const children_iterator & rhs) const { return it == rhs.it; }
                bool operator!=(const children_iterator & rhs) const { return it != rhs.it; }
                CT_NODE_T operator*() { return *it; }
        };
        children_iterator children_begin(CT_NODE_T node) { return children_iterator(children[node].begin()); }
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
         * @return A `vector<tuple>` containing all pairwise leaf distances as `(u, v, distance)`
         */
        std::vector<std::tuple<CT_NODE_T, CT_NODE_T, double>> calc_distance_matrix();
};

// print Newick string (currently recursive)
void compact_tree::print_newick(std::ostream & out, CT_NODE_T node, bool print_semicolon) {
    auto it_begin = children_begin(node); auto it_end = children_end(node);
    for(auto it = it_begin; it != it_end; ++it) { out << ((it == it_begin) ? '(' : ','); print_newick(out, *it, false); }
    if(!is_leaf(node)) { out << ')'; }
    if(label.size() != 0) { out << label[node]; }
    if(length.size() != 0) { out << ':' << length[node]; }
    if(print_semicolon) { out << ';'; }
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
std::vector<std::tuple<CT_NODE_T, CT_NODE_T, double>> compact_tree::calc_distance_matrix() {
    // set things up
    const size_t N = get_num_leaves(); const size_t N_MINUS_1 = N - 1;
    const size_t N_CHOOSE_2 = ((N%2) == 0) ? ((N/2) * N_MINUS_1) : ((N_MINUS_1/2) * N);
    std::vector<std::tuple<CT_NODE_T, CT_NODE_T, double>> dm; dm.reserve(N_CHOOSE_2);
    std::unordered_map<CT_NODE_T, std::unordered_map<CT_NODE_T, double>*> leaf_dists;
    postorder_iterator it_end = postorder_end();
    children_iterator ch_it = children_begin(ROOT_NODE); children_iterator ch_it_2 = children_begin(ROOT_NODE); children_iterator ch_it_end = children_end(ROOT_NODE);
    CT_NODE_T curr; CT_NODE_T child; CT_NODE_T child_2; std::unordered_map<CT_NODE_T, double>* curr_leaf_dists;
    std::unordered_map<CT_NODE_T, double>* child_leaf_dists; std::unordered_map<CT_NODE_T, double>::iterator dist_it; std::unordered_map<CT_NODE_T, double>::iterator dist_it_end;
    std::unordered_map<CT_NODE_T, double>* child_leaf_dists_2; std::unordered_map<CT_NODE_T, double>::iterator dist_it_2; std::unordered_map<CT_NODE_T, double>::iterator dist_it_2_end;

    // calculate pairwise distances
    for(postorder_iterator it = postorder_begin(); it != it_end; ++it) {
        curr = *it; curr_leaf_dists = new std::unordered_map<CT_NODE_T, double>(); leaf_dists.emplace(curr, curr_leaf_dists);
        // for leaves, they have 0 distance to themselves
        if(is_leaf(curr)) {
            curr_leaf_dists->emplace(curr, (double)0.);
        }

        // for internal nodes:
        else {
            // calculate all pairwise distances between leaves below this node
            for(ch_it = children_begin(curr), ch_it_end = children_end(curr); std::next(ch_it) != ch_it_end; ++ch_it) {
                child = *ch_it; child_leaf_dists = leaf_dists[child];
                for(ch_it_2 = std::next(ch_it); ch_it_2 != ch_it_end; ++ch_it_2) {
                    child_2 = *ch_it_2; child_leaf_dists_2 = leaf_dists[child_2];
                    for(dist_it = child_leaf_dists->begin(), dist_it_end = child_leaf_dists->end(); dist_it != dist_it_end; ++dist_it) {
                        for(dist_it_2 = child_leaf_dists_2->begin(), dist_it_2_end = child_leaf_dists_2->end(); dist_it_2 != dist_it_2_end; ++dist_it_2) {
                            dm.emplace_back(std::make_tuple(dist_it->first, dist_it_2->first, dist_it->second + dist_it_2->second + get_edge_length(child) + get_edge_length(child_2)));
                        }
                    }
                }
            }

            // calculate leaf distances to this node
            for(ch_it = children_begin(curr), ch_it_end = children_end(curr); ch_it != ch_it_end; ++ch_it) {
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
        curr = to_copy.top(); to_copy.pop(); auto it_end = children_end(curr.first);
        if(has_lengths) { new_tree.length[curr.second] = length[curr.first]; }
        if(has_labels) { new_tree.label[curr.second] = label[curr.first]; }
        for(auto it = children_begin(curr.first); it != it_end; ++it) {
            to_copy.push(std::make_pair(*it, new_tree.create_child(curr.second, has_labels, has_lengths)));
        }
    }
    return new_tree;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(char* input, bool is_fn, bool store_labels, bool store_lengths, size_t reserve) {
    // reserve space up-front (if given `reserve`) to reduce resizing (save time)
    if(reserve != 0) { parent.reserve(reserve); if(store_lengths) { length.reserve(reserve); } if(store_labels) { label.reserve(reserve); } }

    // set up file input: https://stackoverflow.com/a/17925143/2134991
    int fd = -1;
    size_t bytes_read = 0; size_t i;              // variables to help with reading
    char read_buf[IO_BUFFER_SIZE + 1];            // buffer for reading
    char str_buf[STR_BUFFER_SIZE] = {}; size_t str_buf_i = 0; // helper string buffer
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
            buf = input;
        }

        // handle done reading (!bytes_read) and read failed (bytes_read == -1)
        if(!bytes_read || bytes_read == (size_t)(-1)) {
            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
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
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
                        }
                        return;

                    // go to new child
                    case '(':
                        if(curr_node == NULL_NODE) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
                        }
                        curr_node = create_child(curr_node, store_labels, store_lengths); break;

                    // go to parent
                    case ')':
                        curr_node = parent[curr_node]; break;

                    // go to new sibling
                    case ',':
                        if((curr_node == NULL_NODE) || (parent[curr_node] == NULL_NODE)) {
                            throw std::invalid_argument((is_fn ? ERROR_INVALID_NEWICK_FILE : ERROR_INVALID_NEWICK_STRING) + ": " + input);
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
