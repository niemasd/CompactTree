/**
 * Load a tree topology (no labels or lengths)
 */
#include <chrono>
#include <iostream>
#include "compact_tree.h"
int main(int argc, char** argv) {
    // check user args
    if(argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <tree_file> <schema>" << std::endl; exit(1);
    }
    auto start = std::chrono::system_clock::now();
    compact_tree tree(argv[1], argv[2], false, false);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Duration (s): " << elapsed.count() << std::endl;
    return 0;
}
