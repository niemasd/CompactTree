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
        std::vector<CT_NODE_T> parent;                            // `parent[i]` is the parent of node `i`
        std::vector<std::vector<CT_NODE_T>> children;             // `children[i]` is a `vector` containing the children of node `i`
        std::vector<CT_LENGTH_T> length;                          // `length[i]` is the length of the edge incident to (i.e., going into) node `i`
        std::vector<std::string> label;                           // `label[i]` is the label of node `i`
        std::unordered_map<std::string, CT_NODE_T> label_to_node; // `label_to_node[label]` is the node labeled by `label`

        /**
         * compact_tree helper member variables
         */
        CT_NODE_T tmp_node; // temporary holding variable for nodes (e.g. new child); value should only be used immediately after assigning

        /**
         * Helper function to create a new node and add it as a child to a given node
         * @param parent_node The parent node
         * @return The newly-created child node
         */
        CT_NODE_T create_child(const CT_NODE_T parent_node);

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
        const std::string & get_label(CT_NODE_T node);

        /**
         * Get the label associated with a node
         * @param label The label
         * @return The node labeled by `label`, or -1 if not found
         */
        CT_NODE_T get_node(const std::string & label);
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

// get label from node
const std::string & compact_tree::get_label(CT_NODE_T node) {
    return label[node];
}

// get node from label
CT_NODE_T compact_tree::get_node(const std::string & label) {
    auto tmp = label_to_node.find(label);
    if(tmp == label_to_node.end()) {
        return (CT_NODE_T)(-1);
    }
    return tmp->second;
}

// compact_tree constructor (putting it last because it's super long)
compact_tree::compact_tree(const char* const fn, char* const schema) {
    // convert schema to lowercase
    for(size_t i = 0; schema[i]; ++i) {
        schema[i] = tolower(schema[i]);
    }

    // helper variables
    std::unordered_set<std::string> labels_to_delete; // store labels to delete from `label_to_node` (e.g. duplicates)
    std::unordered_map<std::string, CT_NODE_T>::const_iterator tmp_l2n_it; // helper iterator for looking up in `label_to_node`

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
        size_t bytes_read; char* p; size_t i;         // variables to help with reading
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
            for(p = buf, i = 0; i < bytes_read; ++p, ++i) {
                // currently parsing a comment (ignore for now)
                if(parse_comment) {
                    // reached end of comment
                    if((*p) == ']') {
                        parse_comment = false;
                    }
                }

                // currently parsing an edge length
                else if(parse_length) {
                    switch(*p) {
                        // finished parsing edge length
                        case ',':
                        case ')':
                        case ';':
                            str_buf[str_buf_i] = (char)0;
                            parse_length = false;
                            --i; // need to re-read this character
                            // TODO length[curr_node] = PARSE str_buf AS CT_LENGTH_T
                            break;

                        // edge comment (ignore for now)
                        case '[':
                            parse_comment = true;
                            break;

                        // character within edge length
                        default:
                            str_buf[str_buf_i++] = (*p);
                            break;
                    }
                }

                // currently parsing a node label
                else if(parse_label) {
                    switch(*p) {
                        // finished parsing node label
                        case ':':
                        case ',':
                        case ')':
                        case '\'':
                            str_buf[str_buf_i] = (char)0;
                            parse_label = false;
                            label[curr_node] = str_buf;
                            tmp_l2n_it = label_to_node.find(label[curr_node]);
                            if(tmp_l2n_it == label_to_node.end()) {
                                label_to_node.emplace(label[curr_node], curr_node);
                            } else {
                                labels_to_delete.emplace(label[curr_node]);
                            }
                            break;

                        // node comment (ignore for now)
                        case '[':
                            parse_comment = true;
                            break;

                        // character within node label
                        default:
                            str_buf[str_buf_i++] = (*p);
                            break;
                    }
                }

                // all other symbols
                else {
                    switch(*p) {
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

                        // about to parse a node label
                        case '\'':
                            parse_label = true; str_buf_i = 0;
                            break;

                        // node label
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

    // delete duplicate labels from `label_to_node`
    for(const std::string & curr_label : labels_to_delete) {
        label_to_node.erase(curr_label);
    }

    // TODO DELETE DEBUGGING
    for(auto & pair : label_to_node) {
        std::cout << pair.first << std::endl;
    }
}
#endif
