/*
 * (C) Copyright 2013 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

extern "C" {
#include <jni.h>

#include <util/log.h>
#include <init-log.h>
}

#include "MediaPortManager.h"

static char* LOG_TAG = "NDK-socket-manager";

extern "C" {
	JNIEXPORT jlong JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_takeMediaPortNative(
				JNIEnv* env, jclass clazz, jint port);
	JNIEXPORT jlong JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_takeMediaAddressPortNative(
				JNIEnv* env, jclass clazz, jstring address, jint port);
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPortManager_releaseMediaPortNative(
				JNIEnv* env, jclass clazz, jlong mediaPortRef);
}

using namespace media;

JNIEXPORT jlong JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_takeMediaPortNative(
				JNIEnv* env, jclass clazz, jint port)
{
	MediaPort *mediaPort = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	if (port < 1024)
		mediaPort = MediaPortManager::takeMediaPort();
	else
		mediaPort = MediaPortManager::takeMediaPort(port);

	return (long)mediaPort;
}

JNIEXPORT jlong JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_takeMediaAddressPortNative(
				JNIEnv* env, jclass clazz,
				jstring address, jint port)
{
	const char *p_address = NULL;
	MediaPort *mediaPort = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_address = env->GetStringUTFChars(address, NULL);
	if (p_address == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	mediaPort = MediaPortManager::takeMediaPort(p_address, port);
	env->ReleaseStringUTFChars(address, p_address);

	return (long)mediaPort;
}

JNIEXPORT jint JNICALL
Java_com_kurento_kas_media_ports_MediaPortManager_releaseMediaPortNative(
				JNIEnv* env, jclass clazz, jlong mediaPortRef)
{
	int ret = 0;
	MediaPort *mediaPort;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	mediaPort = (MediaPort*)mediaPortRef;
	if (mediaPort)
		ret = MediaPortManager::releaseMediaPort(mediaPort);

	return ret;
}

