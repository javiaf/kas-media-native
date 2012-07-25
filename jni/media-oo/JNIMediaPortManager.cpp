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
//#include <socket-manager.h>
}

#include "MediaPortManager.h"

static char* LOG_TAG = "NDK-socket-manager";

extern "C" {
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_takeAudioLocalPort(
				JNIEnv* env, jclass clazz, jint audioPort);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_releaseAudioLocalPort(
						JNIEnv* env, jclass clazz);

	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_takeVideoLocalPort(
				JNIEnv* env, jclass clazz, jint videoPort);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_releaseVideoLocalPort(
						JNIEnv* env, jclass clazz);
}

using namespace media;

MediaPort *audioMediaPort;
MediaPort *videoMediaPort;

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_takeAudioLocalPort(
				JNIEnv* env, jclass clazz, jint audioPort) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "takeAudioLocalPort %d", __LINE__);
	if (audioPort < 0)
		audioMediaPort = MediaPortManager::createMediaPort();
	else
		audioMediaPort = MediaPortManager::createMediaPort(audioPort);
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "audioMediaPort->getPort(): %d", audioMediaPort->getPort());
	return audioMediaPort->getPort();
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_releaseAudioLocalPort(
						JNIEnv* env, jclass clazz) {
	int ret = 0;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "releaseAudioLocalPort %d", __LINE__);
	if (audioMediaPort) {
		ret = MediaPortManager::releaseMediaPort(audioMediaPort);
		audioMediaPort = NULL;
	}
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "releaseAudioLocalPort %d", __LINE__);
	return ret;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_takeVideoLocalPort(
				JNIEnv* env, jclass clazz, jint videoPort) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "takeVideoLocalPort %d", __LINE__);
	if (videoPort < 0)
		videoMediaPort = MediaPortManager::createMediaPort();
	else
		videoMediaPort = MediaPortManager::createMediaPort(videoPort);
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "videoMediaPort->getPort(): %d", videoMediaPort->getPort());
	return videoMediaPort->getPort();
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_releaseVideoLocalPort(
						JNIEnv* env, jclass clazz) {
	int ret = 0;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "releaseVideoLocalPort %d", __LINE__);
	if (videoMediaPort) {
		ret = MediaPortManager::releaseMediaPort(videoMediaPort);
		videoMediaPort = NULL;
	}
media_log(MEDIA_LOG_DEBUG, LOG_TAG, "releaseVideoLocalPort %d", __LINE__);
	return ret;
}
