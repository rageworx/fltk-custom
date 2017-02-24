#! /bin/sh
# ====================================================================
# This config64.sh is only for MinGW-W64-64(x86 64bit)
#
# -- notice --
#   To change your build system, must check where placed your
#  64bit MinGW-W64 that supports -m64 inheritance option works.
#   I have seperated version of 32bit and 64bit to building each
#  different environments, so I made this shell script to make build
#  environment to configured with 64bit supported MinGW-W64 in 32bit
#  M-SYS.
# ===================================================================

export MINGW64_PATH=/mingw
export MINGW64_BINS=${MINGW64_PATH}/bin

export MSYS_PATH=${MINGW64_PATH}/msys/1.0
export MSYS_BINS=${MSYS_PATH}/bin

export MINGW6464_PATH=${MINGW64_PATH}/mingw64
export MINGW6464_BINS=${MINGW6464_PATH}/bin
export MINGW6464_LIBS=${MINGW6464_PATH}/lib

export PATH=${MINGW6464_BINS}:${MINGW6464_LIBS}:${MSYS_BINS}

export TOOLCHAIN_PREFIX="x86_64-w64-mingw32-"
export CC="${TOOLCHAIN_PREFIX}gcc"
export CXX="${TOOLCHAIN_PREFIX}g++"
export CFLAGS="-static -static-libstdc++ -static-libgcc -m64"
export CXXFLAGS=${CFLAGS}

./configure --libdir=${MINGW6464_LIBS} --with-abiversion=10304
