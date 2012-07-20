
#ifndef __UTILS_H__
#define __UTILS_H__

#include <jni.h>
#include "libavcodec/avcodec.h"

int get_CodecID_from_VideoCodecTypeEnum(JNIEnv *env,
			jobject videoCodecTypeEnumObj, enum CodecID *codec_id);

#endif /* __UTILS_H__ */
