
#ifndef __JNI_UTILS_H__
#define __JNI_UTILS_H__

#include <jni.h>
#include "libavcodec/avcodec.h"

int get_CodecID_from_VideoCodecTypeEnum(JNIEnv *env,
			jobject videoCodecTypeEnumObj, enum CodecID *codec_id);
int get_CodecID_from_AudioCodecTypeEnum(JNIEnv *env,
			jobject audioCodecTypeEnumObj, enum CodecID *codec_id);

#endif /* __JNI_UTILS_H__ */
