/*
 * Kurento Android Media: Android Media Library based on FFmpeg.
 * Copyright (C) 2011  Tikal Technologies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

extern "C" {
#include <jni.h>

#include <util/log.h>
#include <init-log.h>

#include "util/utils.h"
}

#include "MediaPortManager.h"
#include <AudioTx.h>
#include <VideoTx.h>

using namespace media;

static char* LOG_TAG = "NDK-media-tx";
static AudioTx *aTxObj;
static VideoTx *vTxObj;

extern MediaPort *audioMediaPort;
extern MediaPort *videoMediaPort;

extern "C" {
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_initVideo(JNIEnv* env, jclass clazz,
				jstring outfile, jint width, jint height,
				jint frame_rate_num, jint frame_rate_den,
				jint bit_rate, jint gop_size,
				jobject videoCodecType, jint payload_type);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_putVideoFrame(JNIEnv* env, jclass clazz,
				jbyteArray frame, jint width, jint height, jlong time);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_finishVideo(JNIEnv* env, jclass clazz);

	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_initAudio(JNIEnv* env, jclass clazz,
					jstring outfile, jobject audioCodecType,
					jint sample_rate, jint bit_rate,
					jint payload_type);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_putAudioSamples(JNIEnv* env, jclass clazz,
				jshortArray samples, jint n_samples, jlong time);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_finishAudio(JNIEnv* env, jclass clazz);
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_initVideo(JNIEnv* env, jclass clazz,
				jstring outfile, jint width, jint height,
				jint frame_rate_num, jint frame_rate_den,
				jint bit_rate, jint gop_size,
				jobject videoCodecType, jint payload_type)
{
	int ret;
	const char *f = NULL;

	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = env->GetStringUTFChars(outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

//	ret = init_video_tx(f, width, height, frame_rate_num, frame_rate_den,
//				bit_rate, gop_size, codecId, payload_type,
//				PIX_FMT_NV21);

	enum CodecID codec_id;
//TODO: throw exception
	int ret2 = get_CodecID_from_VideoCodecTypeEnum(env, videoCodecType, &codec_id);
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "ret2: %d, ", ret2);
	vTxObj = new VideoTx(f, width, height, frame_rate_num, frame_rate_den,
				bit_rate, gop_size, codec_id, payload_type,
				PIX_FMT_NV21, videoMediaPort);

	env->ReleaseStringUTFChars(outfile, f);
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "initVideo vTxObj: %p, ", vTxObj);
	//return ret;
	return 0;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_putVideoFrame(JNIEnv* env, jclass clazz,
			jbyteArray frame, jint width, jint height, jlong time)
{
	int ret;
	uint8_t* frame_buf;
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "putVideoFrame vTxObj: %p, ", vTxObj);
	frame_buf = (uint8_t*)(env->GetByteArrayElements(frame, JNI_FALSE));
//	ret = put_video_frame_tx(frame_buf, width, height, time);
	ret = vTxObj->putVideoFrameTx(frame_buf, width, height, time);
	env->ReleaseByteArrayElements(frame, (jbyte*)frame_buf, JNI_ABORT);

	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_finishVideo(JNIEnv* env, jclass clazz)
{
	//return finish_video_tx();
	if (vTxObj) {
		delete vTxObj;
		vTxObj = NULL;
	}
	return 0;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_initAudio(JNIEnv* env, jclass clazz,
					jstring outfile, jobject audioCodecType,
					jint sample_rate, jint bit_rate,
					jint payload_type)
{
	int ret;
	const char *f = NULL;

	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = env->GetStringUTFChars(outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	//ret = init_audio_tx(f, codec_id, sample_rate, bit_rate, payload_type);
	enum CodecID codec_id;
//TODO: throw exception
	int ret2 = get_CodecID_from_AudioCodecTypeEnum(env, audioCodecType, &codec_id);
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "ret2: %d, ", ret2);
	aTxObj = new AudioTx(f, codec_id, sample_rate, bit_rate, payload_type, audioMediaPort);

	env->ReleaseStringUTFChars(outfile, f);

//FIXME: add getFrameSize method
	//return ret;
	return 160;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_putAudioSamples(JNIEnv* env, jclass clazz,
				jshortArray samples, jint n_samples, jlong time)
{
	int ret;
	int16_t *samples_buf;

	samples_buf = (int16_t*)(env->GetShortArrayElements(samples, JNI_FALSE));
//	ret = put_audio_samples_tx(samples_buf, n_samples, time);
	ret = aTxObj->putAudioSamplesTx(samples_buf, n_samples, time);
	env->ReleaseShortArrayElements(samples, samples_buf, 0);

	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_finishAudio(JNIEnv* env, jclass clazz)
{
	if (aTxObj) {
		delete aTxObj;
		aTxObj = NULL;
	}
//	return finish_audio_tx();
	return 0;
}
