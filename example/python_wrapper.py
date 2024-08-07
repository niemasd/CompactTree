#! /usr/bin/env python3
'''
Showcase Python wrappers. Need to first build Python wrapper with `make swig` in root of the repo.
'''

# imports
from compact_tree import compact_tree, traverse_levelorder, traverse_postorder, traverse_preorder
from sys import argv, stderr

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
    print()

    # print number of descendants below each node (including the node itself)
    print("=== Number of Descendants ===")
    num_descendants = [1] * tree.get_num_nodes()
    for node in traverse_postorder(tree):
        if not tree.is_leaf(node):
            num_descendants[node] = 1 + sum(num_descendants[child] for child in tree.get_children(node))
        if tree.get_label(node) != "":
            print("(%s:%d)" % (tree.get_label(node), num_descendants[node]), end=' ')
    print(); print()

    # print unweighted root distances (depths)
    print("=== Unweighted Root Distances ===")
    unweighted_root_dists = [0] * tree.get_num_nodes()
    for node in traverse_levelorder(tree):
        if not tree.is_root(node):
            unweighted_root_dists[node] = unweighted_root_dists[tree.get_parent(node)] + 1
        if tree.get_label(node) != "":
            print("(%s:%d)" % (tree.get_label(node), unweighted_root_dists[node]), end=' ')
    print(); print()

    # print weighted root distances
    print("=== Weighted Root Distances ===")
    weighted_root_dists = [0] * tree.get_num_nodes()
    for node in traverse_preorder(tree):
        if not tree.is_root(node):
            weighted_root_dists[node] = weighted_root_dists[tree.get_parent(node)] + tree.get_edge_length(node)
        if tree.get_label(node) != "":
            print("(%s:%f)" % (tree.get_label(node), weighted_root_dists[node]), end=' ')
    print(); print()

    # print Newick
    print("=== Newick ===")
    print(tree.get_newick())
    print()

    # print topology
    print("=== Topology ===")
    tree.clear_labels()
    tree.clear_edge_lengths()
    print(tree.get_newick())
    print()