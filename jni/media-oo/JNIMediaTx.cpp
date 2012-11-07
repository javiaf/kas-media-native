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

static Lock mutexAudioTx;
static Lock mutexVideoTx;

extern "C" {
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_initVideo(JNIEnv* env, jclass clazz,
				jstring outfile, jint width, jint height,
				jint frame_rate_num, jint frame_rate_den,
				jint bit_rate, jint gop_size,
				jobject videoCodecType, jint payload_type,
				jlong videoMediaPortRef);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_putVideoFrame(JNIEnv* env, jclass clazz,
				jbyteArray frame, jint width, jint height, jlong time);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_finishVideo(JNIEnv* env, jclass clazz);

	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_initAudio(JNIEnv* env, jclass clazz,
					jstring outfile, jobject audioCodecType,
					jint sample_rate, jint bit_rate,
					jint payload_type, jlong audioMediaPortRef);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_putAudioSamples(JNIEnv* env, jclass clazz,
				jshortArray samples, jint n_samples, jlong time);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_tx_MediaTx_finishAudio(JNIEnv* env, jclass clazz);
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_initVideo(JNIEnv* env, jclass clazz,
				jstring outfile, jint width, jint height,
				jint frame_rate_num, jint frame_rate_den,
				jint bit_rate, jint gop_size,
				jobject videoCodecType, jint payload_type,
				jlong videoMediaPortRef)
{
	int ret;
	const char *f = NULL;
	MediaPort *videoMediaPort;

	mutexVideoTx.lock();

	ret = 0;
	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = env->GetStringUTFChars(outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		mutexVideoTx.unlock();
		return -1;
	}

	videoMediaPort = (MediaPort*)videoMediaPortRef;

	enum CodecID codec_id;
//TODO: throw exception
	get_CodecID_from_VideoCodecTypeEnum(env, videoCodecType, &codec_id);
	try {
		vTxObj = new VideoTx(f, width, height, frame_rate_num, frame_rate_den,
				bit_rate, gop_size, codec_id, payload_type,
				PIX_FMT_NV21, videoMediaPort);
	}
	catch(MediaException &e) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "%s", e.what());
		ret = -1;
	}

	env->ReleaseStringUTFChars(outfile, f);
	mutexVideoTx.unlock();
	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_putVideoFrame(JNIEnv* env, jclass clazz,
			jbyteArray frame, jint width, jint height, jlong time)
{
	int ret;
	uint8_t* frame_buf;

	if (!vTxObj) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "No video-tx initiated");
		mutexVideoTx.unlock();
		return -1;
	}

	ret = 0;
	frame_buf = (uint8_t*)(env->GetByteArrayElements(frame, JNI_FALSE));
	try {
		ret = vTxObj->putVideoFrameTx(frame_buf, width, height, time);
	}
	catch(MediaException &e) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "%s", e.what());
		ret = -1;
	}
	env->ReleaseByteArrayElements(frame, (jbyte*)frame_buf, JNI_ABORT);
	mutexVideoTx.unlock();
	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_finishVideo(JNIEnv* env, jclass clazz)
{
	mutexVideoTx.lock();
	if (vTxObj) {
		delete vTxObj;
		vTxObj = NULL;
	}
	mutexVideoTx.unlock();
	return 0;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_initAudio(JNIEnv* env, jclass clazz,
					jstring outfile, jobject audioCodecType,
					jint sample_rate, jint bit_rate,
					jint payload_type, jlong audioMediaPortRef)
{
	int ret;
	const char *f = NULL;
	MediaPort *audioMediaPort;

	mutexAudioTx.lock();

	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = env->GetStringUTFChars(outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		mutexAudioTx.unlock();
		return -1;
	}

	audioMediaPort = (MediaPort*)audioMediaPortRef;

	enum CodecID codec_id;
//TODO: throw exception
	get_CodecID_from_AudioCodecTypeEnum(env, audioCodecType, &codec_id);
	try {
		aTxObj = new AudioTx(f, codec_id, sample_rate, bit_rate,
						payload_type, audioMediaPort);
		ret = aTxObj->getFrameSize();
	}
	catch(MediaException &e) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "%s", e.what());
		ret = -1;
	}

	env->ReleaseStringUTFChars(outfile, f);
	mutexAudioTx.unlock();
	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_putAudioSamples(JNIEnv* env, jclass clazz,
				jshortArray samples, jint n_samples, jlong time)
{
	int ret;
	int16_t *samples_buf;

	mutexAudioTx.lock();
	if (!aTxObj) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "No audio-tx initiated");
		mutexAudioTx.unlock();
		return -1;
	}

	samples_buf = (int16_t*)(env->GetShortArrayElements(samples, JNI_FALSE));
	try {
		ret = aTxObj->putAudioSamplesTx(samples_buf, n_samples, time);
	}
	catch(MediaException &e) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "%s", e.what());
		ret = -1;
	}
	env->ReleaseShortArrayElements(samples, samples_buf, 0);
	mutexAudioTx.unlock();
	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_tx_MediaTx_finishAudio(JNIEnv* env, jclass clazz)
{
	mutexAudioTx.lock();
	if (aTxObj) {
		delete aTxObj;
		aTxObj = NULL;
	}
	mutexAudioTx.unlock();
	return 0;
}
