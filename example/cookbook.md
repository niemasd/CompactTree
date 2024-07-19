# Cookbook

This page contains many examples of CompactTree usage. Rather than showing complete programs (which would require the boilerplate code of the `#include` statement, function header(s), and function return value(s), these examples will be short self-contained code snippets that can be incorporated into larger functions. In all of the examples below, `tree` is a `compact_tree` object.

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
