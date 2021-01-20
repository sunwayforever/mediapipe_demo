%module  inu_stream
%{
#define SWIG_FILE_WITH_INIT
#include "inu_stream.h"
using namespace PyInuStream;
%}
%include "numpy.i"
%init %{
    import_array();
%}
%apply (uint8_t* ARGOUT_ARRAY1, int DIM1) {(uint8_t* output, int output_size)};
%apply int* OUTPUT {int* height, int* width};
%include "inu_stream.h"
