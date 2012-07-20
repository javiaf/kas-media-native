
#include "utils.h"

int
get_CodecID_from_VideoCodecTypeEnum(JNIEnv *env, jobject videoCodecTypeEnumObj,
							enum CodecID *codec_id)
{
	int ret;

	jclass videoCodecTypeEnumClass;
	jmethodID getNameMethod;
	jstring value;
	const char* p_value;

	videoCodecTypeEnumClass = (*env)->FindClass(env, "com/kurento/kas/media/codecs/VideoCodecType");
	getNameMethod = (*env)->GetMethodID(env, videoCodecTypeEnumClass, "name", "()Ljava/lang/String;");

	value = (jstring)(*env)->CallObjectMethod(env, videoCodecTypeEnumObj, getNameMethod);
	p_value = (*env)->GetStringUTFChars(env, value, 0);

	ret = 0;
	if (strcmp(p_value, "H263") == 0)
		*codec_id = CODEC_ID_H263P;
	else if (strcmp(p_value, "MPEG4") == 0)
		*codec_id = CODEC_ID_MPEG4;
	else if (strcmp(p_value, "H264") == 0)
		*codec_id = CODEC_ID_H264;
	else
		ret = -1;

	(*env)->ReleaseStringUTFChars(env, value, p_value);

	return ret;
}
