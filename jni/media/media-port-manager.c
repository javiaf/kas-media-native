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
#include <socket-manager.h>

static char* LOG_TAG = "NDK-socket-manager";

jint
Java_com_kurento_kas_media_ports_MediaPortManager_takeAudioLocalPort(
				JNIEnv* env, jclass class, jint audioPort) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	return take_audio_local_port(audioPort);
}

jint
Java_com_kurento_kas_media_ports_MediaPortManager_releaseAudioLocalPort(
						JNIEnv* env, jclass class) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	return release_audio_local_port();
}

jint
Java_com_kurento_kas_media_ports_MediaPortManager_takeVideoLocalPort(
				JNIEnv* env, jclass class, jint videoPort) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	return take_video_local_port(videoPort);
}

jint
Java_com_kurento_kas_media_ports_MediaPortManager_releaseVideoLocalPort(
						JNIEnv* env, jclass class) {
	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	return release_video_local_port();
}

