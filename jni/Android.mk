LOCAL_PATH := $(NDK_PROJECT_PATH)
include $(CLEAR_VARS)

MEDIA_SOURCES := kc-media-native/media-oo
MEDIA_INCLUDES := $(NDK_PROJECT_PATH)/$(MEDIA_SOURCES)
export EXTERNAL := $(NDK_PROJECT_PATH)/kc-media-native/external

export MY_FFMPEG_SOURCE := $(EXTERNAL)/ffmpeg
export MY_FFMPEG_INSTALL := $(MY_FFMPEG_SOURCE)

export MY_AMR_INSTALL := $(NDK_PROJECT_PATH)/jni/target/opencore-amr_install
export MY_AMR_C_INCLUDE := $(MY_AMR_INSTALL)/include
export MY_AMR_LDLIB := -L$(MY_AMR_INSTALL)/lib -lopencore-amrnb

ifdef ENABLE_X264
	export MY_X264_INSTALL := $(NDK_PROJECT_PATH)/jni/target/x264_install
	export MY_X264_C_INCLUDE := $(MY_X264_INSTALL)/include
	export MY_X264_LDLIB := -L$(MY_X264_INSTALL)/lib -lx264
	LOCAL_CFLAGS += -DUSE_X264
endif

RESULT := $(shell export EXTERNAL=$(EXTERNAL); \
		export MY_FFMPEG_INSTALL=$(MY_FFMPEG_INSTALL); \
		export MY_AMR_INSTALL=$(MY_AMR_INSTALL); \
		export MY_X264_INSTALL=$(MY_X264_INSTALL); \
		$(NDK_PROJECT_PATH)/jni/configure-make-all.sh)

# These need to be in the right order
FFMPEG_LIBS := $(addprefix $(MY_FFMPEG_SOURCE)/, \
	libavformat/libavformat.a \
	libavcodec/libavcodec.a \
	libswscale/libswscale.a \
	libavutil/libavutil.a)

LOCAL_CFLAGS += -Wall -pedantic -std=c99
LOCAL_CPPFLAGS += -D__STDC_CONSTANT_MACROS

LOCAL_LDLIBS += $(FFMPEG_LIBS) $(MY_AMR_LDLIB) $(MY_X264_LDLIB) \
		-llog -lc -lm -ldl -lgcc -lz

LOCAL_C_INCLUDES := 	$(MY_FFMPEG_INSTALL) \
			$(MY_AMR_C_INCLUDE) \
			$(MY_X264_C_INCLUDE) \
			$(MEDIA_INCLUDES) \
			$(MEDIA_INCLUDES)/util \
			$(LOCAL_PATH)/jni/media \
			$(LOCAL_PATH)/jni/media-oo

LOCAL_MODULE := android-media
LOCAL_SRC_FILES :=	$(MEDIA_SOURCES)/util/utils.c $(MEDIA_SOURCES)/util/log.c \
			$(MEDIA_SOURCES)/util/sdp-manager.c \
			$(MEDIA_SOURCES)/Media.cpp \
			$(MEDIA_SOURCES)/MediaPort.cpp $(MEDIA_SOURCES)/MediaPortManager.cpp \
			$(MEDIA_SOURCES)/VideoTx.cpp $(MEDIA_SOURCES)/AudioTx.cpp \
			$(MEDIA_SOURCES)/MediaRx.cpp \
			$(MEDIA_SOURCES)/VideoRx.cpp $(MEDIA_SOURCES)/AudioRx.cpp \
			\
			jni/media/init-log.c \
			jni/media-oo/JNIMediaTx.cpp jni/media-oo/JNIMediaRx.cpp \
			jni/media-oo/JNIMediaPort.cpp jni/media-oo/JNIMediaPortManager.cpp \
			jni/media-oo/util/utils.c

include $(BUILD_SHARED_LIBRARY)
