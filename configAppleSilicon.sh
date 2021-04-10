#! /bin/sh
# --------------------
# Notice 
# --------------------
#   This configure shell script helps doing configure for MacOS x86-86

EFLAGS="--enable-localjpeg --enable-localzlib --enable-localpng"

#AFLAGS="--with-abiversion=10305"
#BSYS="--build=x86_64"

export ARCHFLAGS="-arch x86_64 -arch arm64"

./configure ${EFLAGS} ${AFLAGS} ${BSYS}
