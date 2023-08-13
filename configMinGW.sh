#! /bin/sh
# --------------------
# Notice 
# --------------------
#   This configure shell script helps doing configure for MinGW-W64.

if [ ! -e configure ];then
    echo "configure stopped,"
    echo "need to generate configure first by auotogen.sh !"
    exit 0
fi

DBGOPT=

if [[ "$1"=="debug" ]];then
    DBGOPT+="--enable-debug"
fi

EFLAGS="--enable-localjpeg --enable-localzlib --enable-localpng"
BSYS="--build=x86_64-w64-mingw32"
./configure ${BSYS} ${EFLAGS} ${DBGOPT}
