#! /bin/sh
# --------------------
# Notice 
# --------------------
#   This configure shell script helps doing configure for MinGW-W64.

EFLAGS="--enable-localjpeg --enable-localzlib --enable-localpng"
AFLAGS="--with-abiversion=10305"
BSYS="--build=x86_64-w64-mingw32"
./configure ${BSYS} ${EFLAGS} ${AFLAGS}
