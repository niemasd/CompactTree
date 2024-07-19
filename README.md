# CompactTree
CompactTree is a header-only C++ library for loading and traversing trees. CompactTree is intended for massive trees that can't feasibly fit into memory using other packages (we otherwise recommend [TreeSwift](https://github.com/niemasd/TreeSwift)), and it is intended to be a highly-compact read-only representation of a tree. CompactTree currently only supports trees in the [Newick format](https://en.wikipedia.org/wiki/Newick_format), and it does not support complex annotations (just node labels and edge lengths).

## Usage
Just download [`compact_tree.h`](https://github.com/niemasd/ViralMSA/releases/latest/download/compact_tree.h) into your C++ code base, add `#include "compact_tree.h"` to your code, and use the `compact_tree` class! Here's an example program that loads a Newick file passed as the first command-line argument:

```c++
#include "compact_tree.h"
int main(int argc, char** argv) {
    compact_tree tree(argv[1]);
    return 0;
}
```

This program can be compiled as follows (where `program.cpp` is the name of the C++ source code file, and `program` is the name of the compiled executable):

```bash
g++ -o program program.cpp
```

By default, CompactTree represents nodes (type `CT_NODE_T`) as 32-bit unsigned integers ([`std::uint32_t`](https://cplusplus.com/reference/cstdint/)), and it represents edge lengths (type `CT_LENGTH_T`) using the `float` type, but these can be overridden via compilation arguments:

* `-DCT_NODE_64` will define `CT_NODE_T` as a 64-bit unsigned integer ([`std::uint64_t`](https://cplusplus.com/reference/cstdint/))
* `-DCT_LENGTH_DOUBLE` will define `CT_LENGTH_T` as `double`

For more examples, take a look at the simple example programs in the [`example`](example) folder as well as the [Cookbook](./Cookbook). Full documentation can be found at: https://niema.net/CompactTree

## Citing CompactTree
If you use CompactTree in your work, please cite:

> **Moshiri N** (2024). "CompactTree: A lightweight header-only C++ library for ultra-large phylogenetics." *bioRxiv*. [doi:10.1101/2024.07.15.603593](https://doi.org/10.1101/2024.07.15.603593)
