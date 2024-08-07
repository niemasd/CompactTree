/* name of module */
%module compact_tree

/* ignore nested classes (SWIG doesn't support) */
%ignore compact_tree::children_iterator;
%ignore compact_tree::leaves_iterator;
%ignore compact_tree::levelorder_iterator;
%ignore compact_tree::postorder_iterator;
%ignore compact_tree::preorder_iterator;

/* parts of compact_tree.h to include (everything) */
%{
    #include "compact_tree.h"
%}
%include "compact_tree.h"
