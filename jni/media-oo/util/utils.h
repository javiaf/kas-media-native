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

#ifndef __JNI_UTILS_H__
#define __JNI_UTILS_H__

#include <jni.h>
#include "libavcodec/avcodec.h"

int get_CodecID_from_VideoCodecTypeEnum(JNIEnv *env,
			jobject videoCodecTypeEnumObj, enum CodecID *codec_id);
int get_CodecID_from_AudioCodecTypeEnum(JNIEnv *env,
			jobject audioCodecTypeEnumObj, enum CodecID *codec_id);

#endif /* __JNI_UTILS_H__ */
