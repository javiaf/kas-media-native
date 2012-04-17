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
#include <video-rx.h>
#include <audio-rx.h>

static char* LOG_TAG = "NDK-media-rx";

static jobject video_receiver;
static jclass VideoFrame_class;
static jmethodID video_mid, VideoFrame_init_mid;
static JNIEnv* video_env;

enum {
	QUEUE_SIZE = 20, // FIXME: Coupled with VideoRecorderComponent
	ANDROID_PIX_FMT = PIX_FMT_RGB32,
};

static DecodedFrame adf[QUEUE_SIZE+1];

static int n_frame;
static int current_width, current_height;

static jobject
create_videoframe_obj(JNIEnv *env, DecodedFrame *df)
{
	return (*env)->NewObject(env, VideoFrame_class, VideoFrame_init_mid,
				(jintArray)df->priv_data, df->width, df->height,
				df->time_base.num, df->time_base.den,
				df->pts, df->start_time, df->rx_time);
}

static void
android_put_video_frame_rx(DecodedFrame *decoded_frame)
{
	jobject df_obj;

	df_obj = create_videoframe_obj(video_env, decoded_frame);
	if (df_obj)
		(*video_env)->CallVoidMethod(video_env, video_receiver,
							video_mid, df_obj);
	(*video_env)->DeleteLocalRef(video_env, df_obj);
}

static void
decoded_frame_fill(DecodedFrame *df, int width, int height)
{
	int i, picture_nbytes, current_nbytes;

	// Determine required picture size
	picture_nbytes = avpicture_get_size(ANDROID_PIX_FMT, width, height);
	current_nbytes = avpicture_get_size(ANDROID_PIX_FMT, df->width, df->height);
	media_log(MEDIA_LOG_INFO, LOG_TAG,
			"current_nbytes: %d\tpicture_nbytes: %d",
			current_nbytes, picture_nbytes);

	if (picture_nbytes > current_nbytes) {
		(*video_env)->DeleteLocalRef(video_env, df->priv_data);
		df->priv_data = (jintArray)(*video_env)->NewIntArray(
				video_env, picture_nbytes/sizeof(jint));
		df->buffer = (uint8_t*)(*video_env)->GetIntArrayElements(
						video_env, df->priv_data, NULL);
		(*video_env)->ReleaseIntArrayElements(video_env,
					df->priv_data, (jint*)(df->buffer), 0);
	}

	for (i=0; i<QUEUE_SIZE+1; i++) {
		avpicture_fill((AVPicture*) df->pFrameRGB, df->buffer,
						ANDROID_PIX_FMT, width, height);
	}
}

static DecodedFrame*
android_get_decoded_frame(int width, int height)
{
	DecodedFrame *df = &adf[n_frame];

	if ((df->width != width) || (df->height != height)) {
		decoded_frame_fill(df, width, height);
		current_width = width;
		current_height = height;
		media_log(MEDIA_LOG_INFO, LOG_TAG,
				"current_width: %d\tcurrent_height: %d",
				current_width, current_height);
	}

	n_frame = (n_frame+1) % (QUEUE_SIZE+1);

	return df;
}

static void
android_release_decoded_frame(void)
{
	int i;

	for (i=0; i<QUEUE_SIZE+1; i++) {
		(*video_env)->DeleteLocalRef(video_env, adf[i].priv_data);
		av_free(adf[i].pFrameRGB);
	}
}

static FrameManager android_frame_manager = {
	.pix_fmt			= ANDROID_PIX_FMT,
	.put_video_frame_rx		= android_put_video_frame_rx,
	.get_decoded_frame		= android_get_decoded_frame,
	.release_decoded_frame		= android_release_decoded_frame,
};

jint
Java_com_kurento_kas_media_rx_MediaRx_startVideoRx(JNIEnv* env, jclass class,
				jstring sdp, jint maxDelay, jobject videoReceiver)
{
	int i, ret;
	const char *p_sdp = NULL;

	jclass cls = NULL;

	n_frame = 0;
	current_width = 0;
	current_height = 0;

	video_mid = NULL;
	VideoFrame_init_mid = NULL;
	video_receiver = NULL;
	VideoFrame_class = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = (*env)->GetStringUTFChars(env, sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = (*env)->GetObjectClass(env, videoReceiver);
	video_mid = (*env)->GetMethodID(env, cls, "putVideoFrameRx",
				"(Lcom/kurento/kas/media/rx/VideoFrame;)V");
	if (!video_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
		"putVideoFrameRx(Lcom/kurento/kas/media/rx/VideoFrame;)V no exist");
		ret = -2;
		goto end;
	}

	VideoFrame_class = (*env)->FindClass(env, "com/kurento/kas/media/rx/VideoFrame");
	if (!VideoFrame_class) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
				"com/kurento/kas/media/rx/VideoFrame not found");
		ret = -3;
		goto end;
	}

	VideoFrame_init_mid = (*env)->GetMethodID(env, VideoFrame_class,
							"<init>", "([IIIIIJJJ)V");
	if (!VideoFrame_init_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "init([IIIIIJJJ)V not found");
		ret = -4;
		goto end;
	}

	video_env = env;
	video_receiver = videoReceiver;

	//Allocate AVFrames structures
	for (i=0; i<QUEUE_SIZE+1; i++) {
		adf[i].width = 0;
		adf[i].height = 0;
		adf[i].priv_data = NULL;
		adf[i].pFrameRGB = avcodec_alloc_frame();
		if (adf[i].pFrameRGB == NULL) {
			ret = -3;
			goto end;
		}
	}

	ret = start_video_rx(p_sdp, maxDelay, &android_frame_manager);

end:
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, video_receiver);
	(*env)->DeleteLocalRef(env, VideoFrame_class);
	(*env)->ReleaseStringUTFChars(env, sdp, p_sdp);

	return ret;
}

jint
Java_com_kurento_kas_media_rx_MediaRx_stopVideoRx(JNIEnv* env, jclass class)
{
	return stop_video_rx();
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

	jbuf = (jbyteArray)(*env)->NewByteArray(env, das->size);
	(*env)->SetByteArrayRegion(env, jbuf, 0, das->size, (jbyte*)das->samples);
	das_obj = (*env)->NewObject(env, AudioSamples_class, AudioSamples_init_mid,
				jbuf, das->size,
				das->time_base.num, das->time_base.den,
				das->pts, das->start_time, das->rx_time);
	(*env)->DeleteLocalRef(env, jbuf);

	return das_obj;
}

static void
android_put_audio_samples_rx(DecodedAudioSamples* decoded_audio_samples)
{
	jobject das_obj;

	das_obj = create_audiosamples_obj(audio_env, decoded_audio_samples);
	if (das_obj)
		(*audio_env)->CallVoidMethod(audio_env, audio_receiver,
							audio_mid, das_obj);
	(*audio_env)->DeleteLocalRef(audio_env, das_obj);
}

jint
Java_com_kurento_kas_media_rx_MediaRx_startAudioRx(JNIEnv* env, jclass class,
				jstring sdp, jint maxDelay, jobject audioReceiver)
{
	int ret;
	const char *p_sdp = NULL;

	jclass cls = NULL;

	audio_mid = NULL;
	AudioSamples_init_mid = NULL;
	audio_receiver = NULL;
	AudioSamples_class = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = (*env)->GetStringUTFChars(env, sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = (*env)->GetObjectClass(env, audioReceiver);

	audio_mid = (*env)->GetMethodID(env, cls, "putAudioSamplesRx",
				"(Lcom/kurento/kas/media/rx/AudioSamples;)V");
	if (audio_mid == 0) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
			"(Lcom/kurento/kas/media/rx/AudioSamples;)V no exists");
		ret = -2;
		goto end;
	}

	AudioSamples_class = (*env)->FindClass(env, "com/kurento/kas/media/rx/AudioSamples");
	if (!AudioSamples_class) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
				"com/kurento/kas/media/rx/AudioSamples not found");
		ret = -3;
		goto end;
	}

	AudioSamples_init_mid = (*env)->GetMethodID(env, AudioSamples_class,
							"<init>", "([BIIIJJJ)V");
	if (!AudioSamples_init_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "init([BIIIJJJ)V not found");
		ret = -4;
		goto end;
	}

	audio_env = env;
	audio_receiver = audioReceiver;

	ret = start_audio_rx(p_sdp, maxDelay, &android_put_audio_samples_rx);

end:
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, audio_receiver);
	(*env)->DeleteLocalRef(env, AudioSamples_class);
	(*env)->ReleaseStringUTFChars(env, sdp, p_sdp);

	return ret;
}

jint
Java_com_kurento_kas_media_rx_MediaRx_stopAudioRx(JNIEnv* env, jclass class)
{
	return stop_audio_rx();
}

