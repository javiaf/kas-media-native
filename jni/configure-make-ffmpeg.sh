#!/bin/bash

pushd `dirname $0`
pushd $EXTERNAL/ffmpeg

make clean && make distclean || echo OK

./configure --prefix=${MY_FFMPEG_INSTALL} \
	--target-os=linux \
	--arch=arm \
	--enable-cross-compile \
	--cc=$abi-gcc \
	--cross-prefix=$abi- \
	--nm=$abi-nm \
	--enable-static \
	--disable-shared \
	--enable-neon --enable-pic \
	--disable-amd3dnow --disable-amd3dnowext --disable-mmx --disable-mmx2 --disable-sse --disable-ssse3 \
	--enable-version3 \
	--disable-nonfree \
	--disable-stripping \
	--disable-doc \
	--disable-ffplay \
	--disable-ffmpeg \
	--disable-ffprobe \
	--disable-ffserver \
	--disable-avdevice \
	--disable-avfilter \
	--disable-devices \
	--disable-encoders \
	--enable-encoder=h263p --enable-encoder=mpeg4 \
	--enable-encoder=mp2 --enable-encoder=aac --enable-encoder=pcm_mulaw --enable-encoder=pcm_alaw \
	--extra-cflags="$MY_CFLAGS $AMR_C_EXTRA $X264_C_EXTRA " \
	--extra-ldflags="$MY_LDFLAGS \
			$ARM_LIBO/crtbegin.o $ARM_LIBO/crtend.o \
			$AMR_LD_EXTRA $X264_LD_EXTRA " \
	--extra-libs="-lgcc $AMR_L $X264_L " \
	$AMR_CONFIGURE_OPTS $X264_CONFIGURE_OPTS

make

popd; popd

