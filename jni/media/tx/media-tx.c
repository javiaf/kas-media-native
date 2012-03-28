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

#include <util/log.h>
#include <init-log.h>
#include <jni.h>
#include <video-tx.h>
#include <audio-tx.h>

static char* LOG_TAG = "NDK-media-tx";

jint
Java_com_kurento_kas_media_tx_MediaTx_initVideo(JNIEnv* env, jclass class,
				jstring outfile, jint width, jint height,
				jint frame_rate_num, jint frame_rate_den,
				jint bit_rate, jint gop_size, jint codecId,
				jint payload_type)
{
	int ret;
	const char *f = NULL;

	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = (*env)->GetStringUTFChars(env, outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	ret = init_video_tx(f, width, height, frame_rate_num, frame_rate_den,
				bit_rate, gop_size, codecId, payload_type);

	(*env)->ReleaseStringUTFChars(env, outfile, f);

	return ret;
}

jint
Java_com_kurento_kas_media_tx_MediaTx_putVideoFrame(JNIEnv* env, jclass class,
				jbyteArray frame, jint width, jint height)
{
	int ret;
	uint8_t* frame_buf;

	frame_buf = (uint8_t*)((*env)->GetByteArrayElements(env, frame, JNI_FALSE));
	ret = put_video_frame_tx(frame_buf, width, height);
	(*env)->ReleaseByteArrayElements(env, frame, (jbyte*)frame_buf, 0);

	return ret;
}

jint
Java_com_kurento_kas_media_tx_MediaTx_finishVideo (JNIEnv* env, jclass class)
{
	return finish_video_tx();
}

jint
Java_com_kurento_kas_media_tx_MediaTx_initAudio(JNIEnv* env, jclass class,
					jstring outfile, jint codec_id,
					jint sample_rate, jint bit_rate,
					jint payload_type)
{
	int ret;
	const char *f = NULL;

	if (init_log()!= 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	f = (*env)->GetStringUTFChars(env, outfile, NULL);
	if (f == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	ret = init_audio_tx(f, codec_id, sample_rate, bit_rate, payload_type);

	(*env)->ReleaseStringUTFChars(env, outfile, f);

	return ret;
}

jint
Java_com_kurento_kas_media_tx_MediaTx_putAudioSamples(JNIEnv* env, jclass class,
					jshortArray samples, jint n_samples)
{
	int ret;
	int16_t *samples_buf;

	samples_buf = (int16_t*)((*env)->GetShortArrayElements(env, samples, JNI_FALSE));
	ret = put_audio_samples_tx(samples_buf, n_samples);
	(*env)->ReleaseShortArrayElements(env, samples, samples_buf, 0);

	return ret;
}

jint
Java_com_kurento_kas_media_tx_MediaTx_finishAudio(JNIEnv* env, jclass class)
{
	return finish_audio_tx();
}

