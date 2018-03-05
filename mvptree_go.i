%module mvptree

%{
#include "mvptree_go.h"
%}

/* Let's just grab the original header file here */
%include "mvptree_go.h"

%feature("notabstract") MVPTreeWrap;