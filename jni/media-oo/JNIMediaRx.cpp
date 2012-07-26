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
}

#include "MediaPortManager.h"
#include <AudioRx.h>
#include <VideoRx.h>

using namespace media;

static char* LOG_TAG = "NDK-media-rx";
static AudioRx *aRxObj;
static VideoRx *vRxObj;

//extern MediaPort *audioMediaPort;
//extern MediaPort *videoMediaPort;

extern "C" {
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_rx_MediaRx_startVideoRx(JNIEnv* env, jclass clazz,
				jlong videoMediaPortRef, jstring sdp, jint maxDelay, jobject videoReceiver);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_rx_MediaRx_stopVideoRx(JNIEnv* env, jclass clazz);

	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_rx_MediaRx_startAudioRx(JNIEnv* env, jclass clazz,
				jlong audioMediaPortRef, jstring sdp, jint maxDelay, jobject audioReceiver);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_rx_MediaRx_stopAudioRx(JNIEnv* env, jclass clazz);
}

static jobject video_receiver;
static jclass VideoFrame_class;
static jmethodID put_video_frame_rx_mid, get_frame_buffer_mid, VideoFrame_init_mid;
static JNIEnv* video_env;

enum {
	ANDROID_PIX_FMT = PIX_FMT_RGB32,
};

static DecodedFrame df;

static int current_width, current_height;

static jobject
create_videoframe_obj(JNIEnv *env, DecodedFrame *df)
{
	return env->NewObject(VideoFrame_class, VideoFrame_init_mid,
				(jintArray)df->priv_data, df->width, df->height,
				df->time_base.num, df->time_base.den,
				df->pts, df->start_time, df->rx_time,
				df->encoded_size);
}

static void
android_put_video_frame_rx(DecodedFrame *decoded_frame)
{
	jobject df_obj;

	df_obj = create_videoframe_obj(video_env, decoded_frame);
	if (df_obj)
		video_env->CallVoidMethod(video_receiver,
						put_video_frame_rx_mid, df_obj);
	video_env->DeleteLocalRef(df_obj);
}

static DecodedFrame*
android_get_decoded_frame(int width, int height)
{
	int picture_nbytes;

	// Determine required picture size
	picture_nbytes = avpicture_get_size((enum PixelFormat)ANDROID_PIX_FMT, width, height);

	video_env->DeleteLocalRef((jintArray)df.priv_data);
	df.priv_data = (jintArray)video_env->CallObjectMethod(video_receiver,
					get_frame_buffer_mid, picture_nbytes);
	if (!df.priv_data)
		return NULL;

	df.buffer = (uint8_t*)video_env->GetIntArrayElements((jintArray)df.priv_data, JNI_FALSE);
	video_env->ReleaseIntArrayElements((jintArray)df.priv_data, (jint*)(df.buffer), 0);
	avpicture_fill((AVPicture*) df.pFrameRGB, df.buffer,
				(enum PixelFormat)ANDROID_PIX_FMT, width, height);

	return &df;
}

static void
android_release_decoded_frame(void)
{
	video_env->DeleteLocalRef((jintArray)df.priv_data);
	av_free(df.pFrameRGB);
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_rx_MediaRx_startVideoRx(JNIEnv* env, jclass clazz,
				jlong videoMediaPortRef, jstring sdp, jint maxDelay,
				jobject videoReceiver)
{
	int ret;
	const char *p_sdp = NULL;
	MediaPort *videoMediaPort;

	jclass cls = NULL;

	current_width = 0;
	current_height = 0;

	put_video_frame_rx_mid = NULL;
	get_frame_buffer_mid = NULL;
	VideoFrame_init_mid = NULL;
	video_receiver = NULL;
	VideoFrame_class = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = env->GetStringUTFChars(sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = env->GetObjectClass(videoReceiver);
	put_video_frame_rx_mid = env->GetMethodID(cls, "putVideoFrameRx",
				"(Lcom/kurento/kas/media/rx/VideoFrame;)V");
	if (!put_video_frame_rx_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
		"putVideoFrameRx(Lcom/kurento/kas/media/rx/VideoFrame;)V no exist");
		ret = -2;
		goto end;
	}

	get_frame_buffer_mid = env->GetMethodID(cls, "getFrameBuffer", "(I)[I");
	if (!get_frame_buffer_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "getFrameBuffer(I)[I no exist");
		ret = -3;
		goto end;
	}

	VideoFrame_class = env->FindClass("com/kurento/kas/media/rx/VideoFrame");
	if (!VideoFrame_class) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
				"com/kurento/kas/media/rx/VideoFrame not found");
		ret = -4;
		goto end;
	}

	VideoFrame_init_mid = env->GetMethodID(VideoFrame_class,
							"<init>", "([IIIIIJJJI)V");
	if (!VideoFrame_init_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "init([IIIIIJJJI)V not found");
		ret = -5;
		goto end;
	}

	video_env = env;
	video_receiver = videoReceiver;

	//Allocate AVFrame structure
	df.width = 0;
	df.height = 0;
	df.priv_data = NULL;
	df.pFrameRGB = avcodec_alloc_frame();
	if (df.pFrameRGB == NULL) {
		ret = -6;
		goto end;
	}

	FrameManager android_frame_manager;
	android_frame_manager.pix_fmt = (enum PixelFormat)ANDROID_PIX_FMT;
	android_frame_manager.put_video_frame_rx = android_put_video_frame_rx;
	android_frame_manager.get_decoded_frame = android_get_decoded_frame;
	android_frame_manager.release_decoded_frame =android_release_decoded_frame ;

	videoMediaPort = (MediaPort*)videoMediaPortRef;

	vRxObj = new VideoRx(videoMediaPort, p_sdp, maxDelay, &android_frame_manager);
	vRxObj->start();
	ret = 0;

end:
	env->DeleteLocalRef(cls);
	env->DeleteLocalRef(VideoFrame_class);
	env->ReleaseStringUTFChars(sdp, p_sdp);

	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_rx_MediaRx_stopVideoRx(JNIEnv* env, jclass clazz)
{
	if (vRxObj) {
		vRxObj->stop();
		delete vRxObj;
		vRxObj = NULL;
	}
	return 0;
}

static jobject audio_receiver;
static jclass AudioSamples_class;
static jmethodID audio_mid, AudioSamples_init_mid;
static JNIEnv* audio_env;

static jobject
create_audiosamples_obj(JNIEnv *env, DecodedAudioSamples *das)
{
	jobject das_obj = NULL;
	jbyteArray jbuf = NULL;

	jbuf = (jbyteArray)env->NewByteArray(das->size);
	env->SetByteArrayRegion(jbuf, 0, das->size, (jbyte*)das->samples);
	das_obj = env->NewObject(AudioSamples_class, AudioSamples_init_mid,
				jbuf, das->size,
				das->time_base.num, das->time_base.den,
				das->pts, das->start_time, das->rx_time,
				das->encoded_size);
	env->DeleteLocalRef(jbuf);

	return das_obj;
}

static void
android_put_audio_samples_rx(DecodedAudioSamples* decoded_audio_samples)
{
	jobject das_obj;

	das_obj = create_audiosamples_obj(audio_env, decoded_audio_samples);
	if (das_obj)
		audio_env->CallVoidMethod(audio_receiver,
							audio_mid, das_obj);
	audio_env->DeleteLocalRef(das_obj);
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_rx_MediaRx_startAudioRx(JNIEnv* env, jclass clazz,
				jlong audioMediaPortRef, jstring sdp, jint maxDelay,
				jobject audioReceiver)
{
	int ret = 0;
	const char *p_sdp = NULL;
	MediaPort *audioMediaPort;

	jclass cls = NULL;

	audio_mid = NULL;
	AudioSamples_init_mid = NULL;
	audio_receiver = NULL;
	AudioSamples_class = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = env->GetStringUTFChars(sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = env->GetObjectClass(audioReceiver);

	audio_mid = env->GetMethodID(cls, "putAudioSamplesRx",
				"(Lcom/kurento/kas/media/rx/AudioSamples;)V");
	if (audio_mid == 0) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
			"(Lcom/kurento/kas/media/rx/AudioSamples;)V no exists");
		ret = -2;
		goto end;
	}

	AudioSamples_class = env->FindClass("com/kurento/kas/media/rx/AudioSamples");
	if (!AudioSamples_class) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
				"com/kurento/kas/media/rx/AudioSamples not found");
		ret = -3;
		goto end;
	}

	AudioSamples_init_mid = env->GetMethodID(AudioSamples_class,
							"<init>", "([BIIIJJJI)V");
	if (!AudioSamples_init_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "init([BIIIJJJI)V not found");
		ret = -4;
		goto end;
	}

	audio_env = env;
	audio_receiver = audioReceiver;
	audioMediaPort = (MediaPort*)audioMediaPortRef;

	aRxObj = new AudioRx(audioMediaPort, p_sdp, maxDelay, &android_put_audio_samples_rx);
	aRxObj->start();

end:
	env->DeleteLocalRef(cls);
	env->DeleteLocalRef(audio_receiver);
	env->DeleteLocalRef(AudioSamples_class);
	env->ReleaseStringUTFChars(sdp, p_sdp);

	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_rx_MediaRx_stopAudioRx(JNIEnv* env, jclass clazz)
{
	if (aRxObj) {
		aRxObj->stop();
		delete aRxObj;
		aRxObj = NULL;
	}
	return 0;
}
