#! /bin/sh
# --------------------
# Notice 
# --------------------
#   This configure shell script helps doing configure for llvm-gcc if 
#  your system changed gcc to hpc-gcc.

COMPILER="CC=llvm-gcc CXX=llvm-g++"
FEATURES="--enable-localjpeg  --enable-localzlib --enable-localpng"

./configure ${FEATURES} ${COMPILER} --with-abiversion=10304
