#! /usr/bin/env python3
'''
Showcase Python wrappers. Need to first build Python wrapper with `make swig` in root of the repo.
'''

# imports
from compact_tree import compact_tree # CompactTree wrapper package
from sys import argv, stderr, stdout

# run program
if __name__ == "__main__":
    # check user args
    if len(argv) != 2:
        print("USAGE: %s <tree_file>" % argv[0], file=stderr); exit(1)
    tree = compact_tree(argv[1])

    # print stats
    print("=== Stats ===")
    print("- Number of Nodes: %d" % tree.get_num_nodes())
    print("  - Leaves: %d" % tree.get_num_leaves())
    print("  - Internal: %d" % tree.get_num_internal())
    print("- Total Branch Length: %s" % tree.calc_total_bl())
    print("  - Leaves: %s" % tree.calc_total_bl(include_internal=False))
    print("  - Internal: %s" % tree.calc_total_bl(include_leaves=False))
    print("- Average Branch Length: %s" % tree.calc_avg_bl())
    print("  - Leaves: %s" % tree.calc_avg_bl(include_internal=False))
    print("  - Internal: %s" % tree.calc_avg_bl(include_leaves=False))

    # print Newick
    print("=== Newick ===")
    tree.print_newick(stdout)

    # print topology
    print("=== Topology ===")
    # TODO CLEAR
    tree.print_newick(stdout)
