%module mvptree

%{
#include "mvptree_go.h"
%}

%include <typemaps.i>
%include "std_string.i"
%include "std_vector.i"

namespace std {
   %template(StringVector) vector<string>;
}

// const uint64_t
%typemap(gotype) (const uint64_t) "uint64"

%typemap(out) (vector<string> )
%{
  {
    $input->len = $1->size();
    $input->cap = $1->size();
    $input->array = malloc($input->len * sizeof(unsigned char));
    for (int i = 0; i < $1->size(); i++) {
        ((unsigned char *)$input->array)[i] = (unsigned char)(*$1)[i];
    }
  }
%}

%ignore HammingDistanceComp;

/* Let's just grab the original header file here */
%include "mvptree_go.h"

%feature("notabstract") MVPTreeWrap;