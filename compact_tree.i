/* name of module */
%module compact_tree

/* SWIG includes */
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "typemaps.i"

/* SWIG simple type conversions */
%template(UIntVector) std::vector<unsigned int>;

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
