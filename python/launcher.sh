#!/bin/bash
base_dir=`realpath ${PWD}/..`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${base_dir}/native/inu/lib
export PYTHONPATH=$PYTHONPATH:${base_dir}/native

python -m launcher $*
