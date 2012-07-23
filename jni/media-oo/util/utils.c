
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

int
get_CodecID_from_AudioCodecTypeEnum(JNIEnv *env, jobject audioCodecTypeEnumObj,
							enum CodecID *codec_id)
{
	int ret;

	jclass audioCodecTypeEnumClass;
	jmethodID getNameMethod;
	jstring value;
	const char* p_value;

	audioCodecTypeEnumClass = (*env)->FindClass(env, "com/kurento/kas/media/codecs/AudioCodecType");
	getNameMethod = (*env)->GetMethodID(env, audioCodecTypeEnumClass, "name", "()Ljava/lang/String;");

	value = (jstring)(*env)->CallObjectMethod(env, audioCodecTypeEnumObj, getNameMethod);
	p_value = (*env)->GetStringUTFChars(env, value, 0);

	ret = 0;
	if (strcmp(p_value, "AMR") == 0)
		*codec_id = CODEC_ID_AMR_NB;
	else if (strcmp(p_value, "MP2") == 0)
		*codec_id = CODEC_ID_MP2;
	else if (strcmp(p_value, "AAC") == 0)
		*codec_id = CODEC_ID_AAC;
	else if (strcmp(p_value, "PCMU") == 0)
		*codec_id = CODEC_ID_PCM_MULAW;
	else if (strcmp(p_value, "PCMA") == 0)
		*codec_id = CODEC_ID_PCM_ALAW;
	else
		ret = -1;

	(*env)->ReleaseStringUTFChars(env, value, p_value);

	return ret;
}
