/* name of module */
%module compact_tree

/* SWIG includes */
%include "std_pair.i"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "typemaps.i"

/* SWIG simple type conversions */
%template(VectorUInt) std::vector<unsigned int>;
%template(PairDist) std::pair<std::pair<std::uint32_t,std::uint32_t>,double>;
%template(VectorPairDist) std::vector<std::pair<std::pair<std::uint32_t,std::uint32_t>,double>>;

/* ignore nested classes (SWIG doesn't support them) */
%ignore compact_tree::children_begin;
%ignore compact_tree::children_end;
%ignore compact_tree::children_iterator;
%ignore compact_tree::leaves_begin;
%ignore compact_tree::leaves_end;
%ignore compact_tree::leaves_iterator;
%ignore compact_tree::levelorder_begin;
%ignore compact_tree::levelorder_end;
%ignore compact_tree::levelorder_iterator;
%ignore compact_tree::postorder_begin;
%ignore compact_tree::postorder_end;
%ignore compact_tree::postorder_iterator;
%ignore compact_tree::preorder_begin;
%ignore compact_tree::preorder_end;
%ignore compact_tree::preorder_iterator;

/* ignore things that don't convert properly */
%ignore compact_tree::print_newick;

/* parts of compact_tree.h to include (everything) */
%{
#include "compact_tree.h"
%}
%include "compact_tree.h"

/* helper functions for tree traversals */
%pythoncode %{
from collections import deque
def traverse_leaves(tree):
    for node in range(tree.get_num_nodes()):
        if tree.is_leaf(node):
            yield node
def traverse_levelorder(tree):
    q = deque(); q.append(0)
    while len(q) != 0:
        node = q.popleft(); yield node; q.extend(tree.get_children(node))
def traverse_postorder(tree):
    return range(tree.get_num_nodes()-1, -1, -1)
def traverse_preorder(tree):
    return range(tree.get_num_nodes())
%}
