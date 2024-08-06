# Cookbook

This page contains many examples of CompactTree usage. Rather than showing complete programs (which would require the boilerplate code of `#include` statements, function headers, and function return values), these examples will be short self-contained code snippets that can be incorporated into larger functions/programs (see the [`example`](https://github.com/niemasd/CompactTree/tree/main/example) directory of the CompactTree repository for complete example programs). In all of the examples below, `tree` is a `compact_tree` object.

## Loading a Tree

A tree can be loaded from a Newick file as follows, where `newick_filename` is a C string (`char*`) or [`std::string`](https://cplusplus.com/reference/string/string/) variable containing the filename of the tree:

```cpp
compact_tree tree(newick_filename);
```

The constructor has the following parameters, but only the first is required:

1. `input` — The filename or C string of the tree to load
2. `is_fn` — `true` if `input` is a filename (default), otherwise `false` if `input` is the Newick tree as a C string
3. `store_labels` — `true` to store node labels (default), otherwise `false` (saves memory)
4. `store_lengths` — `true` to store edge lengths (default), otherwise `false` (saves memory)
5. `reserve` — How many nodes to reserve memory for up-front to avoid [`std::vector`](https://cplusplus.com/reference/vector/vector/) resizes (0 by default)

Thus, the above syntax for loading a tree from a Newick file is equivalent to the following:

```cpp
compact_tree tree(newick_filename, true, true, true, 0);
```

A tree can also be loaded from a Newick string as follows, where `newick_string` is a variable containing the Newick string:

```cpp
compact_tree tree(newick_string, false, true, true, 0);
```

Since the default value of `is_fn` is `true`, loading a Newick string requires specifying all constructor arguments.

## Properties of a Tree

### Number of Nodes

The number of nodes in a tree can be retrieved in constant time using the `compact_tree::get_num_nodes` function:

```cpp
size_t num_nodes = tree.get_num_nodes();
```

### Number of Leaves

The number of leaves in a tree can be calculated in linear time using the `compact_tree::get_num_leaves` function:

```cpp
size_t num_leaves = tree.get_num_leaves();
```

After `compact_tree::get_num_leaves` is called the first time, the calculated number of leaves is saved internally, and future calls to `compact_tree::get_num_leaves` will be constant time.

### Number of Internal Nodes

The number of internal nodes in a tree can be calculated in linear time using the `compact_tree::get_num_internal` function:

```cpp
size_t num_internal = tree.get_num_internal();
```

The `compact_tree::get_num_internal` function calculates the number of internal nodes by first calling `compact_tree::get_num_leaves` to calculate the number of leaves, then calling `compact_tree::get_num_nodes` to get the total number of nodes, and finally subtracting the number of leaves from the number of nodes. Thus, the first call to `compact_tree::get_num_internal` *or* `compact_tree::get_num_leaves` will be linear time, and all subsequent calls to *either* function will be constant time.

## Properties of a Node

Nodes of a tree are represented using type `CT_NODE_T`, which is either a 32-bit (default) or 64-bit unsigned integer (i.e., [`std::uint32_t` or `std::uint64_t`](https://cplusplus.com/reference/cstdint/)). The value of a node is guaranteed to be smaller than the values of its children. By extension, `ROOT_NODE` (an alias for the root) is always 0. We also reserve a special alias, `NULL_NODE`, to represent a null node with value `(CT_NODE_T)(-1)`.

### Label

Node labels are represented as type [`std::string`](https://cplusplus.com/reference/string/string/). The label of `node` can be retrieved using the `compact_tree::get_label` function:

```cpp
std::string node_label = tree.get_label(node);
```

If the `compact_tree` object is not storing node labels (i.e., the constructor was called with `store_labels=false`), `compact_tree::get_label` will return the empty string (`""`).

The label of `node` can be changed to `new_label` using the `compact_tree::set_label` function:

```cpp
tree.set_label(node, new_label);
```

If the `compact_tree` object was not storing node labels prior to calling `compact_tree::set_label` (i.e., the constructor was called with `store_labels=false`), node labels will be allocated upon the first call to `compact_tree::set_label`, and all nodes other than `node` will be assigned the empty string (`""`) as their label.

The labels of many nodes can be changed in bulk using the `compact_tree::replace_labels` function:

```cpp
tree.replace_labels(label_map);
```

By default, `compact_tree::replace_labels` will replace *all* matching node labels (including internal nodes). However, in some contexts, you may not want to replace the labels of internal nodes (e.g. if they represent some type of special information). There is a second optional parameter, `include_internal`, that can be set to `false` in order to only replace leaf labels:

```cpp
tree.replace_labels(label_map, false);
```

### Length

Edge lengths are represented as type `CT_LENGTH_T`, which is either `float` (default) or `double`. The length of the incident edge of `node` (i.e., the edge between `node` and its parent) can be retrieved using the `compact_tree::get_edge_length` function:

```cpp
CT_LENGTH_T node_edge_length = tree.get_edge_length(node);
```

If the `compact_tree` object is not storing edge lengths (i.e., the constructor was called with `store_lengths=false`), `compact_tree::get_edge_length` will return 0.

The length of the incident edge of `node` can be changed to `new_length` using the `compact_tree::set_edge_length` function:

```cpp
tree.set_edge_length(node, new_length);
```

If the `compact_tree` object was not storing edge lengths prior to calling `compact_tree::set_edge_length` (i.e., the constructor was called with `store_lengths=false`), edge lengths will be allocated upon the first call to `compact_tree::set_edge_length`, and all edges other than the one incident to `node` will be assigned 0 as their length.

### Parent

The parent of `node` can be retrieved using the `compact_tree::get_parent` function:

```cpp
CT_NODE_T parent = tree.get_parent(node);
```

The root of a tree does not have a parent, so `tree.get_parent(ROOT_NODE)` will return `NULL_NODE`.

### Children

The children of `node` can be retrieved using the `compact_tree::get_children` function:

```cpp
const std::vector<CT_NODE_T> & node_children = tree.get_children(node);
```

Alternatively, the children of `node` can be iterated over using the `compact_tree::children_iterator` class via the `compact_tree::children_begin` and `compact_tree::children_end` functions:

```cpp
for(auto it = tree.children_begin(node); it != tree.children_end(node); ++it) {
    // visit `*it`, which is a `CT_NODE_T`
}
```

Currently, children will be visited in the order they appear in the original Newick string.

#### Adding a Child

While CompactTree does not support substantially restructuring the tree topology because of how it internally represents the tree structure, it *does* allow *adding* a child to `node` using the `compact_tree::add_child` function:

```cpp
CT_NODE_T new_child_1 = tree.add_child(node);
CT_NODE_T new_child_2 = tree.add_child(node, "Niema", 123.456);
```

### All Node Data

While you can retrieve the individual data members of a given node via the getter functions described above, you can also retrieve *all* of the data associated with `node` as an [`std::tuple`](https://cplusplus.com/reference/tuple/tuple/) using the square-brackets operator (`[]`):

```cpp
std::tuple<const std::string*, CT_LENGTH_T, CT_NODE_T, const std::vector<CT_NODE_T>*> node_data = tree[node];
const std::string* node_label_ptr = std::get<0>(node_data);
const std::string node_label = *node_label_ptr;
CT_LENGTH_T node_length = std::get<1>(node_data);
CT_NODE_T node_parent = std::get<2>(node_data);
const std::vector<CT_NODE_T>* node_children_ptr = std::get<3>(node_data);
const std::vector<CT_NODE_T> node_children = *node_children_ptr;
```

However, we recommend only accessing the individual data members of a given node using the aforementioned getter functions, as the square-brackets operator creates an [`std::tuple`](https://cplusplus.com/reference/tuple/tuple/) object, which could add some time/memory overhead to your program.

## Traversing a Tree

Because nodes are represented as unsigned integers, tree traversals can be performed extremely quickly.

### Pre-Order

Because every node has a smaller value than its children, a preorder traversal of a tree with N nodes can be implemented trivially by simply iterating over the integers 0 through N–1:

```cpp
for(CT_NODE_T node = 0; node < N; ++node) {
    // visit `node`
}
```

A preorder traversal can also be performed using the `compact_tree::preorder_iterator` class via the `compact_tree::preorder_begin` and `compact_tree::preorder_end` functions:

```cpp
for(auto it = tree.preorder_begin(); it != tree.preorder_end(); ++it) {
    // visit `*it`, which is a `CT_NODE_T`
}
```

The only guarantee is that a node will be visited before its children. Currently, nodes will be visited in the order they appear in the original Newick string.

### Post-Order

Because every node has a smaller value than its children, a postorder traversal of a tree with N nodes can be implemented trivially by simply iterating over the integers N–1 through 0. However, because `CT_NODE_T` is an unsigned type, you need to be careful with the loop bounds, as decrementing 0 will not yield a negative number (it will wrap around to `NULL_NODE`):

```cpp
for(CT_NODE_T node = N-1; node != NULL_NODE; --node) {
    // visit `node`
}
```

A postorder traversal can also be performed using the `compact_tree::postorder_iterator` class via the `compact_tree::postorder_begin` and `compact_tree::postorder_end` functions:

```cpp
for(auto it = tree.postorder_begin(); it != tree.postorder_end(); ++it) {
    // visit `*it`, which is a `CT_NODE_T`
}
```

The only guarantee is that a node will be visited before its parent. Currently, nodes will be visited in reverse of the order they appear in the original Newick string.

### Level-Order

A level-order traversal can be performed using the `compact_tree::levelorder_iterator` class via the `compact_tree::levelorder_begin` and `compact_tree::levelorder_end` functions:

```cpp
for(auto it = tree.levelorder_begin(); it != tree.levelorder_end(); ++it) {
    // visit `*it`, which is a `CT_NODE_T`
}
```

The only guarantee is that nodes will be visited in increasing order of depth (i.e., number of edges from the root). Currently, nodes with the same depth will be visited in the order they appear in the original Newick string. Note that the copy constructor and the post-increment operator (`it++`) will copy the BFS queue, which is slow and uses extra memory, so avoid both when possible.

### Leaves

A traversal over the leaves of a tree can be performed using the `compact_tree::leaves_iterator` class via the `compact_tree::leaves_begin` and `compact_tree::leaves_end` functions:

```cpp
for(auto it = tree.leaves_begin(); it != tree.leaves_end(); ++it) {
    // visit `*it`, which is a `CT_NODE_T`
}
```

Currently, leaves will be visited in the order they appear in the original Newick string.
