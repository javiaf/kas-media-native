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
}

#include "MediaPort.h"

extern "C" {
	JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPort_getPort(
				JNIEnv* env, jobject thiz, jlong self);
}

using namespace media;

JNIEXPORT jint JNICALL Java_com_kurento_kas_media_ports_MediaPort_getPort(
				JNIEnv* env, jobject thiz, jlong self)
{
	return ((MediaPort*)self)->getPort();
}
