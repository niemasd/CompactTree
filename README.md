# CompactTree
CompactTree is a header-only class for loading and traversing trees. CompactTree is intended for massive trees that can't feasibly fit into memory using other packages (we otherwise recommend [TreeSwift](https://github.com/niemasd/TreeSwift)), and it is intended to be a highly-compact read-only representation of a tree. CompactTree currently only supports trees in the [Newick format](https://en.wikipedia.org/wiki/Newick_format), and it does not support complex annotations (just node labels and edge lengths).

## Usage
Just download [`compact_tree.h`](compact_tree.h) into your C++ code base, add `#include "compact_tree.h"` to your code, and use the `compact_tree` class!

### Example Programs
To demonstrate CompactTree's use, we provide a series of simple example programs:

* **[`load_tree.cpp`](load_tree.cpp)** - Just load a tree
* **[`print_stats.cpp`](print_stats.cpp)** - Print statistics about a tree
