#!/bin/sh

mkdir -p _build/
cd _build
cmake .. \
  -DCMAKE_INSTALL_PREFIX=$PWD/../run \
  -DPYBIND11_DIR=$1

if [ $? -eq 0 ]; then 
  make install $*
fi