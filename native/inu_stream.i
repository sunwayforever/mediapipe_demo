%module  inu_stream
%{
#define SWIG_FILE_WITH_INIT
#include "inu_stream.h"
%}
%include "numpy.i"
%init %{
    import_array();
%}
%include "inu_stream.h"
