#! /bin/sh
# ====================================================================
# This configMac.sh made for Mac OS X cocoa.
# ===================================================================

export CC="llvm-gcc"
export CXX="llvm-g++"
# export CFLAGS="-static -static-libstdc++ -static-libgcc"
# export CXXFLAGS=${CFLAGS}

./configure  --with-abiversion=10304
