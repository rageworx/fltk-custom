#! /bin/sh
# --------------------
# Notice 
# --------------------
#   This configure shell script helps doing configure for llvm-gcc if 
#  your system changed gcc to hpc-gcc.

./configure CC=llvm-gcc CXX=llvm-g++  --with-abiversion=10304
