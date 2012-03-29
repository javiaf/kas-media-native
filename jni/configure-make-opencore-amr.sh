#!/bin/bash

pushd `dirname $0`
pushd $EXTERNAL/opencore-amr

if [ "" == "$MY_AMR_INSTALL" ]; then
  echo "Please set MY_AMR_INSTALL to the location where amr should be installed.";
  exit -1;
fi

aclocal && \
autoheader && \
libtoolize --automake --copy --force && \
automake --add-missing --copy && \
autoconf && \
autoreconf -i

make clean && make distclean || echo OK

export LD="$abi-ld"
export CC="$abi-gcc"
export CXX="$abi-g++"
export CFLAGS="$MY_CFLAGS"
export CXXFLAGS="$MY_CFLAGS"
export LDFLAGS="$MY_LDFLAGS -lsupc++ "

./configure --prefix=$MY_AMR_INSTALL --host=arm

make
make install

popd; popd

