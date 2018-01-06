#!/bin/sh

PWD_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#AFL_USE_ASAN=1
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DKICAD_SCRIPTING=ON \
      -DKICAD_SCRIPTING_MODULES=ON \
      -DKICAD_SCRIPTING_WXPYTHON=ON \
      -DCMAKE_C_COMPILER=afl-clang \
      -DCMAKE_CXX_COMPILER=afl-clang++ \
      ../../

#AFL_USE_ASAN=1
make -j 6
