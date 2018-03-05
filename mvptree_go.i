%module mvptree

%{
#include "mvptree_go.h"
%}

// const uint64_t
%typemap(gotype) (const uint64_t) "uint64"

%ignore HammingDistanceComp;

/* Let's just grab the original header file here */
%include "mvptree_go.h"

%feature("notabstract") MVPTreeWrap;