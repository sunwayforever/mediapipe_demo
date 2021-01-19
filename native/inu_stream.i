%module  inu_stream
%{
#define SWIG_FILE_WITH_INIT
#include "inu_stream.h"
%}
%include "numpy.i"
%init %{
    import_array();
%}
%apply (float* ARGOUT_ARRAY1, int DIM1) {(float* output, int output_size)};
%include "inu_stream.h"
